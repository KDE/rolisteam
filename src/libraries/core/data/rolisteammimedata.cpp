#include "rolisteammimedata.h"

#include <QUrl>

RolisteamMimeData::RolisteamMimeData() : m_data(nullptr)
{
    // m_format = "rolisteam/userlist-item";
}
bool RolisteamMimeData::hasFormat(const QString& mimeType) const
{
    if(mimeType == QStringLiteral("rolisteam/userlist-item"))
    {
        return hasPerson();
    }
    else if(mimeType == QStringLiteral("rolisteam/dice-command"))
    {
        return !m_alias.text().isEmpty();
    }
    return QMimeData::hasFormat(mimeType);
}

void RolisteamMimeData::setPerson(Person* data)
{
    m_data= data;

    if(!data)
        return;

    setText(data->name());
    QList<QUrl> urls;
    urls << QUrl(QString("image://avatar/%1").arg(data->uuid()));
    setUrls(urls);
    setColorData(data->getColor());
}
bool RolisteamMimeData::hasPerson() const
{
    if(m_data != nullptr)
        return true;
    else
        return false;
}
Person* RolisteamMimeData::getData() const
{
    return m_data;
}
DiceShortCut RolisteamMimeData::getAlias() const
{
    return m_alias;
}
void RolisteamMimeData::setAlias(QString key, QString command, bool usedAlias)
{
    m_alias.setText(key);
    m_alias.setCommand(command);
    m_alias.setAlias(usedAlias);
}
