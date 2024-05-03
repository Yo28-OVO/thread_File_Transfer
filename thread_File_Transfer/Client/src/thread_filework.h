#ifndef THREAD_FILEWORK_H
#define THREAD_FILEWORK_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QTcpSocket>
#include <QFile>
class THREAD_FILEWORK : public QObject {
    Q_OBJECT
public:
    enum msgType{FileInfo = 1,FileData};
    explicit THREAD_FILEWORK(QObject *parent = nullptr);
    void thread_StartConnectServer();
    void disConnectServer();
protected:
    void deal_SocketDisconnected();
    void deal_MsgType();
    void thread_get_FileInfo();
    void thread_get_FileData();
private:
    QTcpSocket *socket;
    QSystemTrayIcon *trayIcon;

    QString fileName;
    qint64 fileSize;
    qint64 receiveSize;
    QFile newFile;
    bool isDownloading;
signals:
    void thread_socketDisconnected();
    void thread_connectServer_Succeed();
    void thread_updataProgressBar(int value);
    void thread_sendFile_Finished();
};

#endif // THREAD_FILEWORK_H
