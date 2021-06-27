#include "ipbanaccepter.h"
#include <QDebug>
#include <QHostAddress>

IpBanAccepter::IpBanAccepter() {}

bool IpBanAccepter::isValid(const QMap<QString, QVariant>& data) const
{
    QStringList bannedIp= data["IpBan"].toStringList();
    QString currentIp= data["currentIp"].toString();

    QString currentIpV4;
    QString currentIpV6;
    if(currentIp.count(':') == 8)
    {
        currentIpV6= currentIp.left(currentIp.lastIndexOf(':'));
        currentIpV4= currentIp.mid(currentIp.lastIndexOf(':') + 1);
    }

    // Cut current ip
    bool result
        = ((!bannedIp.contains(currentIp)) && (!bannedIp.contains(currentIpV4)) && (!bannedIp.contains(currentIpV6)));

    // qInfo() << " Ip Ban:" << result << "current IP: " << currentIp;
    return result;
}
