#include "window.h"
#include "thread_filework.h"
#include "./ui_window.h"

Window::Window(QWidget* parent) : QWidget(parent), ui(new Ui::Window)
{
    ui->setupUi(this);
    this->initializeUI();
    this->initializeConnection();
}

Window::~Window()
{
    delete ui;
}

void Window::initializeUI()
{
    this->setWindowIcon(QIcon(":/res/fileIcon.png"));
    QGridLayout* grid = new QGridLayout(this);
    QHBoxLayout* lay = new QHBoxLayout();
    this->connectServer_Button = new CustomButton(QString("连接服务"), Qt::white, this);
    this->progressBar = new QProgressBar(this);
    this->progressBar->setValue(0);
    lay->addWidget(this->connectServer_Button);
    grid->addLayout(lay, 0, 0);
    grid->addWidget(this->progressBar, 1, 0);
    this->setLayout(grid);
}

void Window::initializeConnection()
{
    connect(this->connectServer_Button, &QPushButton::clicked, this, &Window::connectServer);
}

void Window::connectServer()
{
    if (this->connectServer_Button->text() == tr("连接服务"))
    {
        this->startThread();
        emit this->inform_Thread_ConnectServer();
    }
    else if (this->connectServer_Button->text() == tr("断开服务"))
    {
        emit this->inform_Thread_DisconnectedServer();
    }
}

void Window::updateProgressBar(int value)
{
    this->progressBar->setValue(value);
}

void Window::setProgressBarZero()
{
    this->progressBar->setValue(0);
}

void Window::startThread()
{
    // 连接服务
    QThread* thread = new QThread();
    THREAD_FILEWORK* work = new THREAD_FILEWORK();
    work->moveToThread(thread);
    thread->start();
    connect(this, &Window::inform_Thread_ConnectServer, work, &THREAD_FILEWORK::thread_StartConnectServer);
    connect(work, &::THREAD_FILEWORK::thread_connectServer_Succeed, this, [ = ]()
    {
        this->connectServer_Button->setText("断开服务");
    });
    connect(this, &Window::inform_Thread_DisconnectedServer, work, &THREAD_FILEWORK::disConnectServer);
    connect(work, &THREAD_FILEWORK::thread_updataProgressBar, this, &Window::updateProgressBar);
    connect(work, &THREAD_FILEWORK::thread_sendFile_Finished, this, &Window::setProgressBarZero);
    connect(work, &THREAD_FILEWORK::thread_socketDisconnected, this, [ = ]()
    {
        this->connectServer_Button->setText(QString("连接服务"));
        work->deleteLater();
        thread->quit();
        thread->wait();
    });
}
