#include "thread_filework.h"

THREAD_FILEWORK::THREAD_FILEWORK(QObject* parent) : QObject{ parent }
{
    this->fileSize = 0;
    this->receiveSize = 0;
    this->isDownloading = false;
    this->trayIcon = new QSystemTrayIcon(this);
    this->trayIcon->setIcon(QIcon(":/res/fileIcon.png"));
    this->trayIcon->show();
    this->socket = new QTcpSocket(this);
}

void THREAD_FILEWORK::thread_StartConnectServer()
{
    this->socket->connectToHost(QHostAddress("127.0.0.1"), quint16(7502));

    if (!this->socket->waitForConnected(1000))
    {
        this->trayIcon->showMessage("Connect Server Fail", this->socket->errorString());
        this->deal_SocketDisconnected();
        return;
    }

    connect(this->socket, &QTcpSocket::disconnected, this, &THREAD_FILEWORK::deal_SocketDisconnected);
    emit this->thread_connectServer_Succeed();
    this->trayIcon->showMessage("Connected Server Succeed", "连接端口：750242");
    connect(this->socket, &::QTcpSocket::readyRead, this, &THREAD_FILEWORK::deal_MsgType);
}

void THREAD_FILEWORK::disConnectServer()
{
    this->socket->disconnectFromHost();
}

void THREAD_FILEWORK::deal_SocketDisconnected()
{
    emit this->thread_socketDisconnected();
}

void THREAD_FILEWORK::deal_MsgType()
{
    if (this->isDownloading)
    {
        this->thread_get_FileData();
        return;
    }

    QDataStream in(this->socket);
    int type;
    in >> type;

    if (type == msgType::FileInfo)
    {
        this->thread_get_FileInfo();
        this->isDownloading = true;
    }
}

void THREAD_FILEWORK::thread_get_FileInfo()
{
    emit this->thread_sendFile_Finished();
    QDataStream in(this->socket);
    in >>   this->fileName >> this->fileSize;
    this->newFile.setFileName(this->fileName);

    if (!this->newFile.open(QFile::WriteOnly))
    {
        this->trayIcon->showMessage("OPen File Fail", this->newFile.errorString());
        return;
    }

    QByteArray out;
    QDataStream sendType(&out, QIODevice::WriteOnly);
    int type = msgType::FileData;
    sendType << type;
    this->socket->write(out, out.size());
}

void THREAD_FILEWORK::thread_get_FileData()
{
    qint64 socketByteSize = this->socket->bytesAvailable();

    if (socketByteSize < 0)
    {
        return;
    }

    int progress = 0;

    if (this->receiveSize <  this->fileSize)
    {
        progress = (this->receiveSize * 1.0) / (this->fileSize * 1.0) * 100;
        emit this->thread_updataProgressBar(progress);
        QByteArray FILEDATA  = this->socket->read(socketByteSize);
        qint64 WrittenByteSize = this->newFile.write(FILEDATA, FILEDATA.size());
        this->receiveSize += WrittenByteSize;
    }

    if (this->receiveSize == this->fileSize )
    {
        progress = (this->receiveSize * 1.0) / (this->fileSize * 1.0) * 100;
        emit this->thread_updataProgressBar(progress);
        this->fileSize = 0;
        this->receiveSize =  0;
        this->isDownloading = false;
        this->newFile.close();
        return;
    }
}
