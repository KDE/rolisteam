#include "session.h"

Session::Session(const QString& name, QObject* parent) : QObject{parent}, m_name(name) {}

QString Session::name() const
{
    return m_name;
}

void Session::setName(const QString& newName)
{
    if(m_name == newName)
        return;
    m_name= newName;
    emit nameChanged();
}

QByteArray Session::aliases() const
{
    return m_aliases;
}

void Session::setAliases(const QByteArray& newAliases)
{
    if(m_aliases == newAliases)
        return;
    m_aliases= newAliases;
    emit aliasesChanged();
}

QByteArray Session::sheets() const
{
    return m_sheets;
}

void Session::setSheets(const QByteArray& newSheets)
{
    if(m_sheets == newSheets)
        return;
    m_sheets= newSheets;
    emit sheetsChanged();
}

QByteArray Session::macros() const
{
    return m_macros;
}

void Session::setMacros(const QByteArray& newMacros)
{
    if(m_macros == newMacros)
        return;
    m_macros= newMacros;
    emit macrosChanged();
}
