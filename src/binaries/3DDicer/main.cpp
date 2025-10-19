#include <QDirIterator>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <dicelyverse/version.h>

void showResources()
{
    QDirIterator it(":", QDirIterator::Subdirectories);
    while(it.hasNext())
    {
        qDebug() << it.next();
    }
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationDisplayName("DicelyVerse");
    app.setApplicationName("DicelyVerse");
    app.setOrganizationName("rolisteam");
    app.setApplicationVersion(DicelyVerse::FULL_VERSION);

    Q_INIT_RESOURCE(qmake_DicePhysics);

    QQuickStyle::setStyle("DiceStyle");
    QQuickStyle::setFallbackStyle("Material");
    // QQuickStyle::setFallbackStyle("Basic");

    QQmlApplicationEngine engine;
    engine.addImportPath(":/DicePhysics");
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("dicely", "Main");

    return app.exec();
}
