#include "filework.h"

FILEWORK::FILEWORK(qintptr socketDescriptor, QObject *parent) : QObject{ parent } {
    this->socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    this->port = socket->peerPort();
    QString add = this->socket->peerAddress().toString();
    this->address = add.right(9);
    connect(this->socket, &QTcpSocket::disconnected, this, [=]() {
        emit this->clientDisconnected(this->address, this->port);
    });
    connect(this->socket, &QTcpSocket::readyRead, this, &FILEWORK::dealClientMsgType);
    this->timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &FILEWORK::send_client_connected);
    timer->start(100);
}

void FILEWORK::get_File_Info(const QString &filePath) {
    this->_filePath = filePath;
}

void FILEWORK::send_File_Info() {

    qDebug() << this->_filePath;
    this->localFile.setFileName(this->_filePath);
    if (this->localFile.isOpen()) {
        this->localFile.close();
    }

    QFileInfo info(this->_filePath);
    this->_fileName = info.fileName();
    this->_fileSize = info.size();
    QByteArray out;
    QDataStream in(&out, QIODevice::WriteOnly);
    in << int(msgType::FileInfo) << QString(this->_fileName) << qint64(this->_fileSize);
    this->socket->write(out, out.size());

    if (!this->localFile.open(QFile::ReadOnly)) {
        emit this->file_Open_Faild(this->localFile.errorString());
        return;
    }
}

void FILEWORK::send_client_connected() {
    emit this->clientConnected(this->address, this->port);
    this->timer->stop();
}

void FILEWORK::send_File_Data() {
    qint64 loadSize = 1024 * 1024 * 500;
    qint64 alreadySendSize = 0;
    while (alreadySendSize != this->_fileSize) {
        int progress = 0;
        if (alreadySendSize < this->_fileSize) {
            progress = (alreadySendSize * 1.0) / (this->_fileSize * 1.0) * 100;
            qDebug() << QString("当前进度：%1/%2----------%3").arg(alreadySendSize).arg(this->_fileSize).arg(progress);
            QByteArray FILEDATA = this->localFile.read(qMin(this->_fileSize, loadSize));
            qint64 socketSendSize = this->socket->write(FILEDATA, FILEDATA.size());
            this->Delay_MSec_Suspend(50);
            if (!this->socket->waitForBytesWritten(3 * 1000)) {
                emit this->send_FileSize_Fail();
                return;
            }
            alreadySendSize += socketSendSize;
            emit this->send_File_Progress(progress);
        }
        if (alreadySendSize == this->_fileSize) {
            progress = (alreadySendSize * 1.0) / (this->_fileSize * 1.0) * 100;
            qDebug() << QString("当前进度：%1/%2----------%3").arg(alreadySendSize).arg(this->_fileSize).arg(progress);
            emit this->send_File_Progress(progress);
            this->_fileSize = 0;
            this->localFile.close();
            return;
        }
    }
}

void FILEWORK::Delay_MSec_Suspend(unsigned int msec) {
    QTime time = QTime::currentTime();
    QTime NowTime;
    do {
        NowTime = QTime::currentTime();
    } while (time.msecsTo(NowTime) <= msec);
}

void FILEWORK::dealClientMsgType() {
    QDataStream in(this->socket);
    int type;
    in >> type;
    if (type == msgType::FileData) {
        this->send_File_Data();
    } else {
        return;
    }
}
