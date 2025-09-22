#ifndef SESSION_H
#define SESSION_H

#include <QObject>

class Session : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(QByteArray aliases READ aliases WRITE setAliases NOTIFY aliasesChanged FINAL)
    Q_PROPERTY(QByteArray sheets READ sheets WRITE setSheets NOTIFY sheetsChanged FINAL)
    Q_PROPERTY(QByteArray macros READ macros WRITE setMacros NOTIFY macrosChanged FINAL)

public:
    explicit Session(const QString& name= QString(), QObject* parent= nullptr);

    QString name() const;
    void setName(const QString& newName);

    QByteArray aliases() const;
    void setAliases(const QByteArray& newAliases);

    QByteArray sheets() const;
    void setSheets(const QByteArray& newSheets);

    QByteArray macros() const;
    void setMacros(const QByteArray& newMacros);

signals:
    void nameChanged();
    void aliasesChanged();
    void sheetsChanged();
    void macrosChanged();

private:
    QString m_name;
    QByteArray m_aliases;
    QByteArray m_sheets;
    QByteArray m_macros;
};

#endif // SESSION_H
