#ifndef READERSENDER_H
#define READERSENDER_H

#include <QFile>
#include <QHash>
#include <QObject>
#include <QTextStream>

class QTcpSocket;

class ReaderSender : public QObject
{
    Q_OBJECT
public:
    explicit ReaderSender(QObject* parent= nullptr);

    void sendData();
signals:
    void finish();
public slots:
    void readFile();

private:
    QFile m_file;
    QTextStream m_reader;
    QStringList m_data;
    QHash<int, QTcpSocket*> m_hash;
};

#endif // READERSENDER_H
