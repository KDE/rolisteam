#ifndef ROLISTEAMMIMEDATA_H
#define ROLISTEAMMIMEDATA_H

#include <QMimeData>

#include "data/diceshortcut.h"
#include "data/person.h"
#include "media/mediatype.h"
#include <core_global.h>
/**
 * @brief allows drag and drop inside the application scope.
 */
class CORE_EXPORT RolisteamMimeData : public QMimeData
{
    Q_OBJECT
    Q_PROPERTY(Person* person READ person WRITE setPerson NOTIFY personChanged)
    Q_PROPERTY(DiceShortCut alias READ alias NOTIFY aliasChanged)
    Q_PROPERTY(QString npcUuid READ npcUuid WRITE setNpcUuid NOTIFY npcUuidChanged)
    Q_PROPERTY(QList<Core::MediaInfo> mediaInfos READ mediaInfos WRITE setMediaInfos NOTIFY mediaInfosChanged)
public:
    RolisteamMimeData();
    bool hasFormat(const QString& mimeType) const override;
    QStringList formats() const override;

    Person* person() const;
    DiceShortCut alias() const;
    QString npcUuid() const;
    QList<Core::MediaInfo> mediaInfos() const;

public slots:
    void setAlias(QString, QString, bool);
    void setPerson(Person*);
    void setNpcUuid(const QString& str);
    void setMediaInfos(const QList<Core::MediaInfo>& uuid);

signals:
    void personChanged();
    void aliasChanged();
    void npcUuidChanged();
    void mediaInfosChanged();

private:
    Person* m_person= nullptr; /// pointer to stored data
    DiceShortCut m_alias;      /// alias
    QString m_npcUuid;
    QList<Core::MediaInfo> m_mediaInfos;
};
#endif // ROLISTEAMMIMEDATA_H
