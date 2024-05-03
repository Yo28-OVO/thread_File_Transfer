#ifndef WINDOW_H
#define WINDOW_H

#include <QCloseEvent>
#include <QFileDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QList>
#include <QMouseEvent>
#include <QStackedWidget>
#include <QSystemTrayIcon>
#include <QTextBrowser>
#include <QThread>
#include <QWidget>

#include "Custom/custombutton.h"
#include "Custom/customserver.h"
class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget* parent = nullptr);
    ~Window();

protected:
    void initializeUI();
    void initializeConnection();
    void get_File_Info();
    void startServer();
    void NewClientConnected(qintptr socketDescriptor);
    virtual bool eventFilter(QObject* obj, QEvent* event);
    virtual void closeEvent(QCloseEvent* event);

private:
    CustomServer* SERVER;
    // 按钮选项卡
    QFrame* functionFrame;
    CustomButton* fileBtn;
    CustomButton* serverBtn;
    CustomButton* infoBtn;

    QStackedWidget* stackedWidget;
    QWidget* serverWidget;
    CustomButton* start_Server_Btn;
    QWidget* fileWidget;
    CustomButton* select_File_Btn;
    CustomButton* send_File_Btn;
    //
    QSystemTrayIcon* trayIcon;

    QList<QThread*> _clientThread;

signals:
    void send_File_Info_To_Thread(const QString& path);
};
#endif // WINDOW_H
