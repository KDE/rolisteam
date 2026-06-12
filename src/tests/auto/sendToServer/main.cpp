#include "readersender.h"
#include <QCoreApplication>
#include <QTimer>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    ReaderSender reader;
    QObject::connect(&reader, &ReaderSender::finish, &app, &QCoreApplication::quit, Qt::QueuedConnection);
    QTimer::singleShot(1000, &reader, &ReaderSender::readFile);

    return app.exec();
}
