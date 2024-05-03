#ifndef CUSTOMSERVER_H
#define CUSTOMSERVER_H

#include <QTcpServer>
#include <QWidget>
class CustomServer : public QTcpServer {
    Q_OBJECT
    public:
    explicit CustomServer(QWidget *parent = nullptr);

    private:
    void incomingConnection(qintptr socketDescriptor);
    signals:
    void sentSocketDescriptor(qintptr tDescriptor);
};

#endif // CUSTOMSERVER_H
