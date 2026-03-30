#include "network/timeaccepter.h"
#include <QDateTime>
#include <QDebug>
#include <QTime>

TimeAccepter::TimeAccepter() {}

bool TimeAccepter::isValid(const QMap<QString, QVariant>& data) const
{
    auto startStr= data["TimeStart"].toString();
    auto endStr= data["TimeEnd"].toString();

    if(startStr.isEmpty() || endStr.isEmpty())
        return true;

    QDateTime time= QDateTime::currentDateTime();
    const QString format= QStringLiteral("hh:mm");

    QTime start= QTime::fromString(startStr, format);
    QTime end= QTime::fromString(endStr, format);
    QDateTime dateStart;
    dateStart.setDate(time.date());
    dateStart.setTime(start);
    QDateTime dateEnd;
    dateEnd.setDate(time.date());
    dateEnd.setTime(end);

    auto isDayLightChange
        = (dateEnd.time().toString(format) != endStr) || (dateStart.time().toString(format) != startStr);

    if(dateEnd < dateStart && !isDayLightChange)
        dateEnd= dateEnd.addDays(1);
    else if(dateEnd < dateStart)
        dateEnd= dateEnd.addSecs(3600);

    bool result= true;

    if(dateStart.isValid() && dateEnd.isValid() && ((time < dateStart) || (time > dateEnd)))
    {
        result= false;
        qInfo() << QStringLiteral("Connection out of time slot. Connection refused");
    }
    return result;
}
