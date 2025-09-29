#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationDisplayName("DicelyVerse");
    app.setApplicationName("DicelyVerse");
    app.setOrganizationName("rolisteam");

    Q_INIT_RESOURCE(qmake_DicePhysics);

    QQmlApplicationEngine engine;
    engine.addImportPath(":/DicePhysics");
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreationFailed, &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("treeDicer", "Main");

    return app.exec();
}
