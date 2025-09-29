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
    Q_PROPERTY(QByteArray dice3D READ dice3D WRITE setDice3D NOTIFY dice3DChanged FINAL)

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

    QByteArray dice3D() const;
    void setDice3D(const QByteArray& newDice3D);

signals:
    void nameChanged();
    void aliasesChanged();
    void sheetsChanged();
    void macrosChanged();

    void dice3DChanged();

private:
    QString m_name;
    QByteArray m_aliases;
    QByteArray m_sheets;
    QByteArray m_macros;
    QByteArray m_dice3D;
};

#endif // SESSION_H
