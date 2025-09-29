#ifndef PROPERTIESMODEL_H
#define PROPERTIESMODEL_H

#include <QAbstractListModel>

struct FieldInfo
{
    QString key;
    QString value;
};

class PropertiesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum CustomRole
    {
        KeyRole,
        ValueRole
    };
    Q_ENUM(CustomRole)
    explicit PropertiesModel(QObject* parent= nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    QHash<QString, QString> dictionary() const;
    const std::vector<FieldInfo>& infos() const;
    void clear();

public slots:
    void addField(const QString& key= QString(), const QString& value= QString());
    void removeField(int index);

signals:
    void countChanged();

private:
    std::vector<FieldInfo> m_data;
};

#endif // PROPERTIESMODEL_H
