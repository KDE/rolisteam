#ifndef CHANNELMODEL_H
#define CHANNELMODEL_H

#include <QAbstractItemModel>
#include <QMimeData>
#include <QObject>

#include "network/channel.h"
#include "network/serverconnection.h"
#include "network_global.h" // NETWORK_EXPORT

class NETWORK_EXPORT ClientMimeData : public QMimeData
{
    Q_OBJECT
public:
    ClientMimeData();
    void addClient(ServerConnection* m, const QModelIndex);
    const QMap<QModelIndex, ServerConnection*>& getList() const;
    virtual bool hasFormat(const QString& mimeType) const;

private:
    QMap<QModelIndex, ServerConnection*> m_clientList;
};
/**
 * @brief The ChannelModel class
 */
class NETWORK_EXPORT ChannelModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(bool isServer READ isServer CONSTANT)
    Q_PROPERTY(bool admin READ admin WRITE setAdmin NOTIFY adminChanged FINAL)
public:
    ChannelModel(bool isServer= false);
    ~ChannelModel();

    virtual int rowCount(const QModelIndex& parent) const;
    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;

    bool setData(const QModelIndex& index, const QVariant& value, int role);

    QString addChannel(const QString& id, const QString& name, const QString& description, const QByteArray& password,
                       const QString& parentId);
    bool addConnectionToChannel(QString chanId, ServerConnection* client);

    void readSettings();
    void writeSettings();

    bool isServer() const;

    bool addConnectionToDefaultChannel(ServerConnection* client);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    bool hasChildren(const QModelIndex& parent) const;

    void kick(const QString&, bool isAdmin, const QString& senderId);

    TreeItem* getItemById(QString id) const;
    ServerConnection* getServerConnectionById(QString id) const;

    bool isGM(const QString& id, const QString& chanId) const;

    QModelIndex channelToIndex(Channel* channel);

    void setLocalPlayerId(const QString& id);

    void removeChild(QString id);
    QStringList mimeTypes() const;
    Qt::DropActions supportedDropActions() const;
    QMimeData* mimeData(const QModelIndexList& indexes) const;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

    void cleanUp();
    void emptyChannelMemory();
    void renameChannel(const QString& id, const QString& value);
    bool moveClient(Channel* origin, const QString& id, Channel* dest);

    const QList<QPointer<TreeItem>>& modelData();
    void resetData(QList<TreeItem*> data);
    bool admin() const;
    void setAdmin(bool newAdmin);

signals:
    void totalSizeChanged(quint64);
    void localPlayerGMChanged(QString id);
    void modelChanged();
    void channelNameChanged(QString id, QString name);
    void adminChanged();
    void userLeftChannel(const QString& channel, const QString& userId);
    void userHasJoinedChannel(const QString& id, const QString& userId);

public slots:
    void setChannelMemorySize(Channel* chan, quint64);

protected slots:
    QModelIndex addChannelToIndex(Channel* channel, const QModelIndex& parent);
    bool addChannelToChannel(Channel* child, Channel* parent);

protected:
    bool moveMediaItem(QList<ServerConnection*> items, const QModelIndex& parentToBe, int row,
                       QList<QModelIndex>& formerPosition);

    void appendChannel(Channel* channel);
    bool localIsGM() const;

private:
    std::pair<quint64, QString> convert(quint64 size) const;

private:
    QList<QPointer<TreeItem>> m_root;
    std::map<Channel*, quint64> m_sizeMap;
    QString m_defaultChannel;
    QString m_localPlayerId;
    bool m_admin= false;
    bool m_shield= false;
    bool m_server= false;
};

#endif // CHANNELMODEL_H
