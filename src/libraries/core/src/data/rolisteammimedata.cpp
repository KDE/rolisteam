#include "data/rolisteammimedata.h"

#include "media/mediatype.h"
#include <QUrl>

RolisteamMimeData::RolisteamMimeData() {}

bool RolisteamMimeData::hasFormat(const QString& mimeType) const
{
    if(mimeType == Core::mimedata::MIME_KEY_PERSON_DATA)
    {
        return m_person;
    }
    else if(mimeType == Core::mimedata::MIME_KEY_DICE_ALIAS_DATA)
    {
        return !m_alias.text().isEmpty();
    }
    else if(mimeType == Core::mimedata::MIME_KEY_MEDIA_UUID)
    {
        return !m_mediaInfos.isEmpty();
    }
    else if(mimeType == Core::mimedata::MIME_KEY_NPC_ID)
    {
        return !m_npcUuid.isEmpty();
    }
    return QMimeData::hasFormat(mimeType);
}

QStringList RolisteamMimeData::formats() const
{
    QStringList res= QMimeData::formats();

    if(m_person)
        res << Core::mimedata::MIME_KEY_PERSON_DATA;

    if(!m_alias.text().isEmpty())
        res << Core::mimedata::MIME_KEY_DICE_ALIAS_DATA;

    if(!m_mediaInfos.isEmpty())
        res << Core::mimedata::MIME_KEY_MEDIA_UUID;

    if(!m_npcUuid.isEmpty())
        res << Core::mimedata::MIME_KEY_NPC_ID;

    return res;
}

void RolisteamMimeData::setPerson(Person* data)
{
    if(data == m_person)
        return;
    m_person= data;
    emit personChanged();

    if(!m_person)
        return;

    setText(data->name());
    QList<QUrl> urls;
    urls << QUrl(QString("image://avatar/%1").arg(data->uuid()));
    setUrls(urls);
    setColorData(data->getColor());
    if(data->hasAvatar())
        setImageData(QImage::fromData(data->avatar()));
}

void RolisteamMimeData::setNpcUuid(const QString& str)
{
    if(m_npcUuid == str)
        return;
    m_npcUuid= str;
    emit npcUuidChanged();
}

void RolisteamMimeData::setMediaInfos(const QList<Core::MediaInfo>& uuid)
{
    if(m_mediaInfos == uuid)
        return;
    m_mediaInfos= uuid;
    emit mediaInfosChanged();
}

Person* RolisteamMimeData::person() const
{
    return m_person;
}
DiceShortCut RolisteamMimeData::alias() const
{
    return m_alias;
}

QString RolisteamMimeData::npcUuid() const
{
    return m_npcUuid;
}

QList<Core::MediaInfo> RolisteamMimeData::mediaInfos() const
{
    return m_mediaInfos;
}

void RolisteamMimeData::setAlias(QString key, QString command, bool usedAlias)
{
    m_alias.setText(key);
    m_alias.setCommand(command);
    m_alias.setAlias(usedAlias);
    emit aliasChanged();
}
