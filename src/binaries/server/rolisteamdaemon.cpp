#include "rolisteamdaemon.h"
#include <QDebug>
#include <QFile>
#include <QTime>

#include <QCoreApplication>
#include <QFileInfo>
#include <QSettings>

#include "media/networktype.h"

RolisteamDaemon::RolisteamDaemon(QObject* parent) : QObject(parent), m_logController(new LogController(true, this))
{
    m_server.reset(new RServer(m_parameters, false));
    qRegisterMetaType<RServer::ServerState>();
}

bool RolisteamDaemon::readConfigFile(QString filepath)
{
    if(!QFileInfo::exists(filepath))
        return false;

    QSettings settings(filepath, QSettings::IniFormat);
    namespace nc= network::configkeys;

    int port= settings.value(nc::port).toInt();
    auto password= QByteArray::fromBase64(settings.value(nc::serverPassword).toByteArray());
    QString range= settings.value(network::configkeys::ipRange).toString();
    QString ipBan= settings.value(network::configkeys::ipBan).toString();
    QString connectionMax= settings.value(network::configkeys::connectionMax).toString();
    QString timeStart= settings.value(network::configkeys::timeStart).toString();
    QString timeEnd= settings.value(nc::timeEnd).toString();
    QString ipMode= settings.value(nc::ipMode).toString();
    auto adminPassword= QByteArray::fromBase64(settings.value(nc::adminPassword).toByteArray());
    int threadCount= settings.value(nc::threadCount).toInt();
    int channelCount= settings.value(nc::channelCount).toInt();
    int timeToRetry= settings.value(nc::timeToRetry).toInt();
    int tryCount= settings.value(nc::tryCount).toInt();
    int logLevel= settings.value(nc::logLevel).toInt();
    QString maxMemorySize= settings.value(nc::memorySize).toString();
    bool deepInspectionLog= settings.value(nc::deepInspection).toBool();

    if(threadCount <= 0)
        return false;

    QString pathToLog= settings.value(nc::logFile).toString();

    quint64 memorySize= 0;
    quint64 factor= 0;
    if(maxMemorySize.endsWith("G"))
    {
        factor= 1024 * 1024 * 1024;
    }
    else if(maxMemorySize.endsWith("M"))
    {
        factor= 1024 * 1024;
    }
    maxMemorySize= maxMemorySize.remove(maxMemorySize.length() - 1, 1);

    memorySize= factor * maxMemorySize.toULongLong();

    auto listIpBan= ipBan.split(",", Qt::SkipEmptyParts);

    /*if(deepInspectionLog)
    {
        m_logController->listenObjects(m_server.get());
    }*/
    m_logController->setLogLevel(static_cast<LogController::LogLevel>(logLevel));

    LogController::StorageModes modes= LogController::Console;

    if(!pathToLog.isEmpty())
    {
        m_logController->setCurrentPath(pathToLog);
        modes|= LogController::File;
    }

    m_logController->setCurrentModes(modes);

    m_parameters.insert(network::configkeys::port, port);
    m_parameters.insert(network::configkeys::serverPassword, password);
    m_parameters.insert(network::configkeys::ipRange, range);
    m_parameters.insert(network::configkeys::ipBan, listIpBan);
    m_parameters.insert(network::configkeys::connectionMax, connectionMax);
    m_parameters.insert(network::configkeys::timeStart, timeStart);
    m_parameters.insert(network::configkeys::timeEnd, timeEnd);
    m_parameters.insert(network::configkeys::adminPassword, adminPassword);
    m_parameters.insert(network::configkeys::ipMode, ipMode);                    // v4 v6 any
    m_parameters.insert(network::configkeys::threadCount, threadCount);          // thread count
    m_parameters.insert(network::configkeys::channelCount, channelCount);        // channel count
    m_parameters.insert(network::configkeys::timeToRetry, timeToRetry);          // TimeToRetry
    m_parameters.insert(network::configkeys::tryCount, tryCount);                // TimeToRetry
    m_parameters.insert(network::configkeys::logLevel, logLevel);                // loglevel
    m_parameters.insert(network::configkeys::logFile, pathToLog);                // logpath
    m_parameters.insert(network::configkeys::deepInspection, deepInspectionLog); // logpath
    m_parameters.insert(network::configkeys::memorySize, memorySize);            // max memory size

    return true;
}

void RolisteamDaemon::start()
{
    connect(&m_thread, &QThread::started, m_server.get(), &RServer::listen);
    connect(
        m_server.get(), &RServer::eventOccured, m_logController,
        [this](const QString& msg, LogController::LogLevel type)
        { m_logController->manageMessage(msg, QStringLiteral("Server"), type); },
        Qt::QueuedConnection);
    // connect(m_server.get(), &RServer::completed, &m_thread, &QThread::quit);
    /*connect(m_server.get(), &RServer::stateChanged, this,
            [this]()
            {
                if(m_server->state() == RServer::Finished)
                    m_thread.quit();
            });*/
    connect(&m_thread, &QThread::finished, this,
            [this]()
            {
                if(m_restart)
                {
                    qApp->exit(0);
                }
                m_restart= false;
            });

    m_server->moveToThread(&m_thread);

    m_thread.start();
}

void RolisteamDaemon::createEmptyConfigFile(QString filepath)
{
    QSettings settings(filepath, QSettings::IniFormat);

    settings.setValue("port", 6660);
    settings.setValue("ServerPassword", QStringLiteral(""));
    settings.setValue("IpRange", QStringLiteral(""));
    settings.setValue("IpBan", QStringLiteral(""));
    settings.setValue("ConnectionMax", 50);
    settings.setValue("TimeStart", QStringLiteral(""));
    settings.setValue("TimeEnd", QStringLiteral(""));
    settings.setValue("IpMode", QStringLiteral(""));
    settings.setValue("ThreadCount", 1);
    settings.setValue("ChannelCount", 1);
    settings.setValue("TimeToRetry", 100);
    settings.setValue("TryCount", 10);
    settings.setValue("LogLevel", 1);
    settings.setValue("LogFile", QStringLiteral(""));
    settings.setValue("DeepInspectionLog", false);
    settings.setValue("AdminPassword", QStringLiteral(""));
    settings.setValue("MaxMemorySize", QStringLiteral(""));

    settings.sync();
}

void RolisteamDaemon::restart()
{
    m_restart= true;
    QMetaObject::invokeMethod(m_server.get(), &RServer::close, Qt::QueuedConnection);
}

void RolisteamDaemon::stop()
{
    QMetaObject::invokeMethod(m_server.get(), &RServer::close, Qt::QueuedConnection);
}
