#include "network/upnp/upnpnat.h"

#include <QNetworkReply>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QUdpSocket>
#include <QUrl>
#include <QXmlStreamReader>
#include <inja.hpp>
#include <json.hpp>

#include "iohelper.h"

/******************************************************************
** Discovery Defines                                                 *
*******************************************************************/
// clang-format off
#define HTTPMU_HOST_ADDRESS "239.255.255.250"
#define HTTPMU_HOST_ADDRESS_V6 "FF02::1"
#define HTTPMU_HOST_PORT 1900
#define SEARCH_REQUEST_STRING "M-SEARCH * HTTP/1.1\n"            \
                              "ST:UPnP:rootdevice\n"             \
                              "MX: 3\n"                          \
                              "Man:\"ssdp:discover\"\n"          \
                              "HOST: 239.255.255.250:1900\n"     \
                                                            "\n"


/******************************************************************
** Device and Service  Defines                                                 *
*******************************************************************/
namespace key
{
constexpr auto deviceType1{"urn:schemas-upnp-org:device:InternetGatewayDevice"};
constexpr auto deviceType2{"urn:schemas-upnp-org:device:WANDevice"};
constexpr auto deviceType3{"urn:schemas-upnp-org:device:WANConnectionDevice"};

constexpr auto serviceTypeWanIP{"urn:schemas-upnp-org:service:WANIPConnection"};
constexpr auto serviceTypeWANPPP{"urn:schemas-upnp-org:service:WANPPPConnection"};

constexpr auto envelop{"://src/network/upnp/mapport.xml"};
}
// clang-format on
//*********************************************************************************

UpnpNat::UpnpNat(QObject* parent) : QObject(parent)
{
    connect(&m_manager, &QNetworkAccessManager::finished, this,
            [this](QNetworkReply* reply)
            {
                if(m_status == UpnpNat::NAT_STAT::NAT_READY)
                {
                    processAnswer(reply);
                }
                else
                {
                    processXML(reply);
                }
            });
    qDebug() << "UPNP creation";
}

UpnpNat::~UpnpNat()= default;

void UpnpNat::discovery()
{
    m_serviceType.clear();
    m_describeUrl.clear();
    m_controlUrl.clear();
    m_baseUrl.clear();
    m_serviceDescribeUrl.clear();
    m_error.clear();
    m_mappingInfo.clear();
    setStatus(NAT_STAT::NAT_DISCOVERY);
    m_udpSocketV4.reset(new QUdpSocket(this));

    QHostAddress broadcastIpV4(HTTPMU_HOST_ADDRESS);

    m_udpSocketV4->bind(QHostAddress(QHostAddress::AnyIPv4), 0);
    QByteArray datagram(SEARCH_REQUEST_STRING);

    connect(m_udpSocketV4.get(), &QTcpSocket::readyRead, this,
            [this]()
            {
                QByteArray datagram;
                while(m_udpSocketV4->hasPendingDatagrams())
                {
                    datagram.resize(int(m_udpSocketV4->pendingDatagramSize()));
                    m_udpSocketV4->readDatagram(datagram.data(), datagram.size());
                }

                QString result(datagram);
                auto start= result.indexOf("http://");

                if(start < 0)
                {
                    setError(tr("Unable to read the beginning of server answer"));
                    setStatus(NAT_STAT::NAT_ERROR);
                    return;
                }

                auto end= result.indexOf("\r", start);
                if(end < 0)
                {
                    setError(tr("Unable to read the end of server answer"));
                    setStatus(NAT_STAT::NAT_ERROR);
                    return;
                }

                m_describeUrl= result.sliced(start, end - start);

                setStatus(NAT_STAT::NAT_FOUND);
                m_udpSocketV4->close();
            });

    connect(m_udpSocketV4.get(), &QUdpSocket::errorOccurred, this,
            [this](QAbstractSocket::SocketError)
            {
                setError(m_udpSocketV4->errorString());
                setStatus(NAT_STAT::NAT_ERROR);
            });

    m_udpSocketV4->writeDatagram(datagram, broadcastIpV4, HTTPMU_HOST_PORT);
}

void UpnpNat::requestDescription()
{
    setStatus(NAT_STAT::NAT_GETDESCRIPTION);
    QNetworkRequest request;
    request.setUrl(QUrl(m_describeUrl));
    m_manager.get(request);
}

void UpnpNat::processXML(QNetworkReply* reply)
{
    auto data= reply->readAll();
    if(data.isEmpty())
    {
        setError(tr("Description file is empty"));
        setStatus(NAT_STAT::NAT_ERROR);
        return;
    }

    setStatus(NAT_STAT::NAT_DESCRIPTION_FOUND);

    QXmlStreamReader xml(data);
    bool isType1= false;
    bool isType2= false;
    bool isType3= false;

    auto goToNextCharacter= [&xml]()
    {
        while(!xml.isCharacters() || xml.isWhitespace())
            xml.readNext();
    };

    while(!xml.atEnd())
    {
        xml.readNext();
        if(xml.name() == QLatin1String("URLBase"))
        {
            goToNextCharacter();
            m_baseUrl= xml.text().toString();
        }
        if(xml.name() == QLatin1String("deviceType"))
        {
            goToNextCharacter();
            auto text= xml.text().toString();

            if(text.startsWith(key::deviceType1))
                isType1= true;
            if(text.startsWith(key::deviceType2))
                isType2= true;
            if(text.startsWith(key::deviceType3))
                isType3= true;
        }
        if(xml.name() == QLatin1String("serviceType"))
        {
            goToNextCharacter();
            auto serviceType= xml.text().toString();
            if((serviceType.contains(key::serviceTypeWANPPP) || serviceType.contains(key::serviceTypeWanIP))
               && m_serviceType.isEmpty())
            {
                m_serviceType= serviceType;
            }
        }
        if(xml.name() == QLatin1String("controlURL") && !m_serviceType.isEmpty() && m_controlUrl.isEmpty())
        {
            goToNextCharacter();
            m_controlUrl= xml.text().toString();
        }
    }

    if(m_baseUrl.isEmpty())
    {
        auto index= m_describeUrl.indexOf("/", 7);
        if(index < 0)
        {
            setError(tr("Fail to get base_URL from XMLNode \"URLBase\" or describe_url.\n"));
            return;
        }
        m_baseUrl= m_describeUrl.sliced(0, index);
    }

    if(!isType1 || !isType2 || !isType3)
    {
        setError(tr("Fail to find proper service type: %1 %2 %3").arg(isType1).arg(isType2).arg(isType3));
        return;
    }

    // make the complete control_url;
    if(!m_controlUrl.startsWith("http://", Qt::CaseInsensitive))
        m_controlUrl= m_baseUrl + m_controlUrl;
    if(!m_serviceDescribeUrl.startsWith("http://", Qt::CaseInsensitive))
        m_serviceDescribeUrl= m_baseUrl + m_serviceDescribeUrl;

#ifdef QT_DEBUG
    qDebug() << "##############";
    qDebug() << "Service: " << m_serviceType;
    qDebug() << "describe:" << m_describeUrl;
    qDebug() << "Control:" << m_controlUrl;
    qDebug() << "Base:" << m_baseUrl;
    qDebug() << "service url:" << m_serviceDescribeUrl;
    qDebug() << "##############" << isType1 << isType2 << isType3;
#endif

    setStatus(NAT_STAT::NAT_READY);
}

void UpnpNat::addPortMapping(const QString& description, const QString& destination_ip, unsigned short int port_ex,
                             unsigned short int port_in, const QString& protocol)
{
    Q_UNUSED(port_ex);
    inja::json subdata;
    subdata["service"]= m_serviceType.toStdString();
    subdata["port"]= port_in;
    subdata["ip"]= destination_ip.toStdString();
    subdata["protocol"]= protocol.toStdString();
    subdata["description"]= description.toStdString();

    auto text= QByteArray::fromStdString(inja::render(utils::IOHelper::loadFile(key::envelop).toStdString(), subdata));

    QNetworkRequest request;
    request.setUrl(QUrl(m_controlUrl));
    QHttpHeaders headers;
    headers.append(QHttpHeaders::WellKnownHeader::ContentType, "text/xml;  charset=\"utf-8\"");
    headers.append("SOAPAction", QString("\"%1#AddPortMapping\"").arg(m_serviceType));
    request.setHeaders(headers);
    m_manager.post(request, text);
}

void UpnpNat::processAnswer(QNetworkReply* reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        setError(tr("Something went wrong: %1").arg(reply->errorString()));
        setStatus(NAT_STAT::NAT_ERROR);
        return;
    }
    setStatus(NAT_STAT::NAT_ADD);
}

void UpnpNat::setStatus(NAT_STAT status)
{
    if(m_status == status)
        return;
    m_status= status;
    emit statusChanged();
}

void UpnpNat::setError(const QString& error)
{
    if(m_error == error)
        return;
    m_error= error;
    emit errorChanged();
}

void UpnpNat::setLocalIp(const QString& ip)
{
    if(m_localIp == ip)
        return;

    m_localIp= ip;
    emit localIpChanged();
}
QString UpnpNat::localIp() const
{
    return m_localIp;
}

UpnpNat::NAT_STAT UpnpNat::status() const
{
    return m_status;
}
