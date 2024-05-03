#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H
#define LEAVE_WHITE_STYLE "background-color:#5DE2E7;border-radius:8px;"
#define ENTER_WHITE_STYLE "background-color:#27A2A7;border-radius:8px;"
#define LEAVE_OTHER_STYLE "background-color:#C3DABE;border-radius:7px;"
#define ENTER_OTHER_STYLE "background-color:#BCC8B9;border-radius:7px;"

#include <QColor>
#include <QEnterEvent>
#include <QPushButton>
#include <QTableWidget>
#include <QTextBrowser>
class CustomButton : public QPushButton {
    Q_OBJECT
    public:
    explicit CustomButton(const QString &text, QColor color, QWidget *parent = nullptr);

    protected:
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;

    private:
    QColor textColor;
};

#endif // CUSTOMBUTTON_H
