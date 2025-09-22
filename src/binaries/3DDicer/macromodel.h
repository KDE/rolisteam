#ifndef MACROSMODEL_H
#define MACROSMODEL_H

#include <QAbstractListModel>

struct MacroInfo
{
    QString name;
    QString command;
};

class MacrosModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum CustomRole
    {
        NameRole= Qt::UserRole + 1,
        CommandRole
    };
    Q_ENUM(CustomRole)
    explicit MacrosModel(QObject* parent= nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    const std::vector<MacroInfo>& macros() const;

public slots:
    void addMacro(const QString& key= QString(), const QString& value= QString());
    void removeMacro(int index);

private:
    std::vector<MacroInfo> m_data;
};

#endif // MACROSMODEL_H
