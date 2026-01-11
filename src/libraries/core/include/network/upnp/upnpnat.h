#ifndef UPNPNAT_H
#define UPNPNAT_H

#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QObject>
#include <memory.h>

#include "network_global.h"

class QUdpSocket;
class NETWORK_EXPORT UpnpNat : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(QString localIp READ localIp WRITE setLocalIp NOTIFY localIpChanged)
public:
    enum class NAT_STAT
    {
        NAT_IDLE= 0,
        NAT_DISCOVERY,
        NAT_FOUND,
        NAT_GETDESCRIPTION,
        NAT_DESCRIPTION_FOUND,
        NAT_READY,
        NAT_ADD,
        NAT_ERROR
    };
    UpnpNat(QObject* parent= nullptr);
    virtual ~UpnpNat();
    QString error() const { return m_error; }
    QString localIp() const;
    NAT_STAT status() const;

public slots:
    void discovery(); // find router
    void addPortMapping(const QString& description, const QString& destination_ip, unsigned short int port_ex,
                        unsigned short int port_in, const QString& protocol); // add port mapping
    void requestDescription();
    void setLocalIp(const QString& ip);

signals:
    void errorChanged();
    void statusChanged();
    void localIpChanged();

private slots:
    void setStatus(UpnpNat::NAT_STAT status);
    void setError(const QString& error);
    void processXML(QNetworkReply* reply);
    void processAnswer(QNetworkReply* reply);

private:
    NAT_STAT m_status{NAT_STAT::NAT_IDLE};
    QString m_serviceType;
    QString m_describeUrl;
    QString m_controlUrl;
    QString m_baseUrl;
    QString m_serviceDescribeUrl;
    QString m_error;
    QString m_mappingInfo;
    QString m_localIp;
    QNetworkAccessManager m_manager;
    std::unique_ptr<QUdpSocket> m_udpSocketV4;
};

#endif
