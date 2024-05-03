#ifndef WINDOW_H
#define WINDOW_H

#include <QGridLayout>
#include <QHBoxLayout>
#include <QProgressBar>
#include <QSystemTrayIcon>
#include <QThread>
#include <QWidget>

#include "Custom/custombutton.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class Window;
}
QT_END_NAMESPACE

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(QWidget* parent = nullptr);
    ~Window();

protected:
    void initializeUI();
    void initializeConnection();
    void connectServer();
    void updateProgressBar(int value);
    void setProgressBarZero();
private:
    Ui::Window* ui;
    void startThread();
    CustomButton* connectServer_Button;
    QProgressBar* progressBar;
signals:
    void inform_Thread_ConnectServer();
    void inform_Thread_DisconnectedServer();
};
#endif // WINDOW_H
