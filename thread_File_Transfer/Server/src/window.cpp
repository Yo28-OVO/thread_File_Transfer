#include "window.h"

#include "thread_File_Work/filework.h"
Window::Window(QWidget* parent)
    : QWidget(parent)
{
    this->initializeUI();
    this->initializeConnection();
}

Window::~Window()
{
}

void Window::initializeUI()
{
    this->setWindowTitle(QString("文件传输"));
    this->setWindowIcon(QIcon(":/res/titleIcon2.png"));
    this->setFixedSize(800, 600);
    QGridLayout* grid = new QGridLayout(this);
    this->fileBtn = new CustomButton(QString("文件"), Qt::black, this);
    this->serverBtn = new CustomButton(QString("服务"), Qt::white, this);
    this->infoBtn = new CustomButton(QString("信息"), Qt::black, this);
    grid->setHorizontalSpacing(40);
    grid->addWidget(this->serverBtn, 0, 0);
    grid->addWidget(this->fileBtn, 1, 0);
    grid->addWidget(this->infoBtn, 2, 0);
    grid->addWidget(new CustomButton(QString("测试"), Qt::black, this), 3, 0);
    grid->addWidget(new CustomButton(QString("测试"), Qt::black, this), 4, 0);
    //
    this->stackedWidget = new QStackedWidget(this);
    this->serverWidget = new QWidget(this->stackedWidget);
    this->start_Server_Btn = new CustomButton(QString("启动服务"), Qt::white, this->serverWidget);
    QHBoxLayout* lay = new QHBoxLayout(this->serverWidget);
    lay->addWidget(this->start_Server_Btn);
    this->stackedWidget->addWidget(this->serverWidget);
    //
    this->fileWidget = new QWidget(this->stackedWidget);
    this->select_File_Btn = new CustomButton(QString("选择文件"), Qt::white, this->fileWidget);
    this->send_File_Btn = new CustomButton(QString("发送文件"), Qt::black, this->fileWidget);
    QHBoxLayout* fileWidget_lay = new QHBoxLayout(this->fileWidget);
    fileWidget_lay->addWidget(this->select_File_Btn);
    fileWidget_lay->addWidget(this->send_File_Btn);
    this->stackedWidget->addWidget(this->fileWidget);
    grid->addWidget(this->stackedWidget, 0, 1, 5, 5);
    this->setLayout(grid);

    this->stackedWidget->setCurrentIndex(0);

    this->serverBtn->installEventFilter(this);
    this->fileBtn->installEventFilter(this);

    this->trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/res/titleIcon2.png"));
    trayIcon->show();

    this->SERVER = new CustomServer(this);
}

void Window::initializeConnection()
{
    // 选择文件
    connect(this->select_File_Btn, &CustomButton::clicked, this, &Window::get_File_Info);
    connect(this->start_Server_Btn, &CustomButton::clicked, this, &Window::startServer);
}

void Window::get_File_Info()
{
    QString filepath = QFileDialog::getOpenFileName(this, "选择文件", "/");
    if (filepath.isEmpty())
    {
        this->trayIcon->showMessage("文件不能为空", "请检测文件路径", QSystemTrayIcon::Critical);
    }
    emit this->send_File_Info_To_Thread(filepath);
}

void Window::startServer()
{
    if (this->start_Server_Btn->text() == tr("启动服务"))
    {
        if (this->SERVER->isListening())
        {
            this->SERVER->close();
        }
        if (this->SERVER->listen(QHostAddress::Any, quint16(7502)))
        {
            this->start_Server_Btn->setText("关闭服务");
            this->trayIcon->showMessage("Start Server Succeed ！", QString("Listening Port：%1").arg(this->SERVER->serverPort()), QSystemTrayIcon::Information);
            connect(this->SERVER, &CustomServer::sentSocketDescriptor, this, &Window::NewClientConnected);
        }
        else
        {
            this->trayIcon->showMessage("Start Server Fail ！", this->SERVER->errorString(), QSystemTrayIcon::Critical);
            return;
        }
    }
    else if (this->start_Server_Btn->text() == tr("关闭服务"))
    {
        this->start_Server_Btn->setText("启动服务");
        this->SERVER->close();
        this->trayIcon->showMessage("Server ", "Server Is Closed!");
        return;
    }
}

void Window::NewClientConnected(qintptr socketDescriptor)
{
    QThread* thread = new QThread;
    FILEWORK* work = new FILEWORK(socketDescriptor);
    work->moveToThread(thread);
    thread->start();

    this->_clientThread.append(thread);

    connect(this, &Window::send_File_Info_To_Thread, work, &FILEWORK::get_File_Info);
    connect(work, &FILEWORK::file_Open_Faild, this, [=](const QString& errorOPen)
            { this->trayIcon->showMessage("File  Opened Fali", errorOPen); });
    connect(work, &FILEWORK::clientConnected, this, [=](const QString& address, quint16 port)
            { this->trayIcon->showMessage("New Client Connected !", QString("address：%1 port：%2").arg(address).arg(port)); });
    //
    connect(work, &FILEWORK::clientDisconnected, this, [=](const QString& address, const quint16 port)
            {
                this->trayIcon->showMessage("Client Disconnected", QString("address：%1 Port：%2").arg(address).arg(port));
                work->deleteLater();
                thread->quit();
                thread->wait(); });
    connect(this->send_File_Btn, &QPushButton::clicked, work, &FILEWORK::send_File_Info);
    connect(work,&FILEWORK::destroyed,this,[=](){
        qDebug() << "destroyed";
    });
}

bool Window::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == this->serverBtn)
    {
        QMouseEvent* e = (QMouseEvent*)(event);
        if (e->button() == Qt::LeftButton && e->type() == QMouseEvent::MouseButtonPress)
        {
            this->stackedWidget->setCurrentIndex(0);
        }
    }
    if (obj == this->fileBtn)
    {
        QMouseEvent* e = (QMouseEvent*)(event);
        if (e->button() == Qt::LeftButton && e->type() == QMouseEvent::MouseButtonPress)
        {
            this->stackedWidget->setCurrentIndex(1);
        }
    }
    return QWidget::eventFilter(obj, event);
}

void Window::closeEvent(QCloseEvent* event)
{
    foreach (QThread* thread, this->_clientThread)
    {
        if (thread->isRunning())
        {
            thread->exit(0);
            thread->quit();
            thread->wait();
            this->_clientThread.removeOne(thread);
        }
    }

    this->_clientThread.clear();
}
