#ifndef FILEWORK_H
#define FILEWORK_H

#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QTcpSocket>
#include <QTimer>
class FILEWORK : public QObject {
    Q_OBJECT
    public:
    enum msgType { FileInfo = 1, FileData };
    explicit FILEWORK(qintptr socketDescriptor, QObject *parent = nullptr);
    void get_File_Info(const QString &filePath);
    void send_File_Info();
    void send_client_connected();
    signals:
    void clientDisconnected(const QString &address, const quint16 port);
    void clientConnected(const QString &address, const quint16 port);
    void file_Open_Faild(const QString &errorOpen);
    void send_FileSize_Fail();
    void send_File_Progress(int value);

    private:
    void send_File_Data();
    void Delay_MSec_Suspend(unsigned int msec);
    void dealClientMsgType();
    QTcpSocket *socket;
    QString address;
    quint16 port;

    QString _filePath;
    QString _fileName;
    qint64 _fileSize;
    QFile localFile;
    QTimer *timer;
};

#endif // FILEWORK_H
