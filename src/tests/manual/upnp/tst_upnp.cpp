#include "network/upnp/upnpnat.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);
    UpnpNat nat;

    QObject::connect(&nat, &UpnpNat::statusChanged,
                     [&nat]()
                     {
                         switch(nat.status())
                         {
                         case UpnpNat::NAT_STAT::NAT_IDLE:
                         case UpnpNat::NAT_STAT::NAT_DISCOVERY:
                         case UpnpNat::NAT_STAT::NAT_GETDESCRIPTION:
                         case UpnpNat::NAT_STAT::NAT_DESCRIPTION_FOUND:
                             break;
                         case UpnpNat::NAT_STAT::NAT_FOUND:
                             nat.requestDescription();
                             break;
                         case UpnpNat::NAT_STAT::NAT_READY:
                             nat.addPortMapping("RolisteamServer", "192.168.1.2", 6660, 6660, "TCP");
                             break;
                         case UpnpNat::NAT_STAT::NAT_ADD:
                             qDebug() << "[Upnp] Successful mapping port: " << 6660;
                             nat.deleteLater();
                             break;
                         case UpnpNat::NAT_STAT::NAT_ERROR:
                             qDebug() << "[Upnp]" << nat.error();
                             nat.deleteLater();
                             break;
                         }
                     });
    nat.discovery();

    return app.exec();
}
