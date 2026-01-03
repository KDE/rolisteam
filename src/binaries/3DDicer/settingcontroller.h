#ifndef SETTINGCONTROLLER_H
#define SETTINGCONTROLLER_H

#include <QAbstractListModel>
#include <QObject>
#include <QQmlEngine>

#include "session.h"
#include "sessionmodel.h"

class SettingController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(SessionModel* sessions READ sessions CONSTANT)
    Q_PROPERTY(int currentSessionIndex READ currentSessionIndex WRITE setCurrentSessionIndex NOTIFY
                   currentSessionIndexChanged FINAL)
    Q_PROPERTY(Session* currentSession READ currentSession NOTIFY currentSessionIndexChanged FINAL)
public:
    explicit SettingController(QObject* parent= nullptr);

    SessionModel* sessions() const;

    int currentSessionIndex() const;
    void setCurrentSessionIndex(int newCurrentSessionIndex);
    int sessionIndex(const QString& index) const;
    int sessionCount() const;
    QString sessionName() const;

    Session* currentSession() const;

signals:
    void currentSessionIndexChanged();
    void sessionCountChanged();

private:
    std::unique_ptr<SessionModel> m_sessions;
    int m_currentSessionIndex;
};

#endif // SETTINGCONTROLLER_H
