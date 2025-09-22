#include "settingcontroller.h"

SettingController::SettingController(QObject* parent) : QObject{parent}, m_sessions{new SessionModel} {}

SessionModel* SettingController::sessions() const
{
    return m_sessions.get();
}

int SettingController::currentSessionIndex() const
{
    return m_currentSessionIndex;
}

void SettingController::setCurrentSessionIndex(int newCurrentSessionIndex)
{
    if(m_currentSessionIndex == newCurrentSessionIndex)
        return;
    m_currentSessionIndex= newCurrentSessionIndex;
    emit currentSessionIndexChanged();
}
int SettingController::sessionCount() const
{
    return m_sessions->rowCount();
}
QString SettingController::sessionName() const
{
    auto it= currentSession();
    if(!it)
        return "default";
    else
        return it->name();
}

Session* SettingController::currentSession() const
{
    return m_sessions->session(m_currentSessionIndex);
}
