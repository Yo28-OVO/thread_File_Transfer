#ifndef CUSTOMSERVER_H
#define CUSTOMSERVER_H

#include <QObject>
#include <QTcpServer>
class CustomServer : public QTcpServer {
    Q_OBJECT
    public:
    explicit CustomServer(QObject *parent = nullptr);

    protected:
    virtual void incomingConnection(qintptr socketDescriptor) override;

    signals:
    void sentSocketDescriptor(qintptr tDescriptor);
};

#endif // CUSTOMSERVER_H
