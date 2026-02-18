#include <QDebug>
#include <QMessageLogger>

#include "common/logcategory.h"
#include "media/networktype.h"
#include "network/rserver.h"
#include "network/serverconnectionmanager.h"
#include "network/timeaccepter.h"

RServer::RServer(const QMap<QString, QVariant>& parameter, bool internal, QObject* parent)
    : QTcpServer(parent), m_corConnection(new TimeAccepter()), m_data(parameter), m_internal(internal)
{
    if(m_data.contains(network::configkeys::threadCount))
        m_threadCount= m_data.value(network::configkeys::threadCount, m_threadCount).toInt();
    initServerConnections();
}

RServer::~RServer()
{
    complete();
    emit eventOccured(QStringLiteral("Server Destructor"), LogController::Debug);
    for(auto& info : m_threadPool)
    {
        info.m_thread->quit();
        info.m_thread->wait(1000);
    }
}

void RServer::initServerConnections()
{
    m_connectionsManager.reset(new ServerConnectionManager(m_data));
    m_updater.reset(new ServerManagerUpdater(m_connectionsManager.get(), m_internal));
    if(m_connectionThread)
        m_connectionThread->exit();
    m_connectionThread.reset(new QThread);

    connect(this, &RServer::accepting, m_connectionsManager.get(), &ServerConnectionManager::accept,
            Qt::QueuedConnection);
    connect(m_connectionsManager.get(), &ServerConnectionManager::finished, this, &RServer::complete,
            Qt::QueuedConnection);

    for(int i= 0; i < m_threadCount; ++i)
    {
        ThreadInfo info{new QThread(this), 0};
        m_threadPool.append(info);
        info.m_thread->start();
    }

    m_connectionsManager->moveToThread(m_connectionThread.get());
    m_updater->moveToThread(m_connectionThread.get());

    m_connectionThread->start();
}

bool RServer::listen()
{
    if(!QTcpServer::listen(QHostAddress::Any, m_data[network::configkeys::port].toInt()))
    {
        emit eventOccured(errorString(), LogController::LogLevel::Error);
        return false;
    }

    emit eventOccured(tr("[Server] Start Listening"), LogController::Info);

    setState(RServer::Listening);

    return true;
}

void RServer::close()
{
    if(state() == RServer::Idle)
        return;
    QTcpServer::close();
    // m_connectionsManager->quit() invoke method
    setState(RServer::Idle);
}

bool RServer::internal() const
{
    return m_internal;
}

int RServer::threadCount() const
{
    return m_threadCount;
}

RServer::ServerState RServer::state() const
{
    return m_state;
}

void RServer::incomingConnection(qintptr descriptor)
{
    emit eventOccured(QStringLiteral("incoming Connection"), LogController::Debug);
    if(!m_corConnection->runAccepter(m_data))
        return;

    accept(descriptor);
}

void RServer::accept(qintptr descriptor)
{
    if(m_threadPool.isEmpty())
    {
        emit eventOccured(QStringLiteral("threadpool is empty - server is stopped"), LogController::Error);
        return;
    }

    auto it= std::min_element(std::begin(m_threadPool), std::end(m_threadPool),
                              [](const ThreadInfo& a, const ThreadInfo& b)
                              { return a.m_connectionCount < b.m_connectionCount; });

    if(it == std::end(m_threadPool))
    {
        emit eventOccured(QStringLiteral("No thread available - server is stopped"), LogController::Error);
        return;
    }

    it->m_connectionCount++;
    ServerConnection* connection= new ServerConnection();
    connection->moveToThread(it->m_thread);

    emit accepting(descriptor, connection);
}

void RServer::setState(const ServerState& state)
{
    if(state == m_state)
        return;

    m_state= state;
    emit stateChanged(m_state);
}

void RServer::complete()
{
    if(!m_connectionThread)
    {
        emit eventOccured(QStringLiteral("exiting complete there was no thread!"), LogController::Warning);
        return;
    }

    m_connectionsManager.release();

    m_connectionThread->quit();
    m_connectionThread->wait();

    m_connectionThread.release();

    emit eventOccured(QStringLiteral("Rserver Complete"), LogController::Info);
    // emit completed();
}
