/***************************************************************************
 *	Copyright (C) 2026 by Renaud Guezennec                               *
 *   http://www.rolisteam.org/contact                                      *
 *                                                                         *
 *   This software is free software; you can redistribute it and/or modify *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef NETWORKTYPE_H
#define NETWORKTYPE_H

#include <QObject>

#include <network_global.h>

namespace network
{

Q_NAMESPACE_EXPORT(NETWORK_EXPORT)

namespace configkeys
{
constexpr auto port{"port"};
constexpr auto serverPassword{"ServerPassword"};
constexpr auto adminPassword{"AdminPassword"};
constexpr auto ipRange{"IpRange"};
constexpr auto ipBan{"IpBan"};
constexpr auto connectionMax{"ConnectionMax"};
constexpr auto timeStart{"TimeStart"};
constexpr auto timeEnd{"TimeEnd"};
constexpr auto ipMode{"IpMode"};
constexpr auto threadCount{"ThreadCount"};
constexpr auto channelCount{"ChannelCount"};
constexpr auto timeToRetry{"TimeToRetry"};
constexpr auto tryCount{"TryCount"};
constexpr auto logLevel{"LogLevel"};
constexpr auto logFile{"LogFile"};
constexpr auto deepInspection{"DeepInspectionLog"};
constexpr auto memorySize{"MemorySize"};
} // namespace configkeys

namespace upnp
{
constexpr auto upnpEnabled{"Upnp_enabled"};
constexpr auto upnpLocalIp{"Upnp_localIp"};
} // namespace upnp
} // namespace network
#endif // NETWORKTYPE_H
