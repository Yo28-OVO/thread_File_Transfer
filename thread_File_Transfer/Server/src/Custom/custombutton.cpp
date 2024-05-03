#include "custombutton.h"

CustomButton::CustomButton(const QString &text, QColor color, QWidget *parent) {
    this->setFixedSize(200, 35);
    this->setText(text);
    this->setParent(parent);
    // 字体颜色
    QPalette pale = this->palette();
    pale.setColor(QPalette::ButtonText, color);
    this->setPalette(pale);
    this->textColor = color;
    QFont font;
    font.setBold(true);
    this->setFont(font);
    if (this->textColor == Qt::white) {
        this->setStyleSheet(LEAVE_WHITE_STYLE);
    } else {
        this->setStyleSheet(LEAVE_OTHER_STYLE);
    }
}

void CustomButton::enterEvent(QEnterEvent *event) {
    if (this->textColor == Qt::white) {
        this->setStyleSheet(ENTER_WHITE_STYLE);
    } else {
        this->setStyleSheet(ENTER_OTHER_STYLE);
    }
    event->ignore();
}

void CustomButton::leaveEvent(QEvent *event) {
    if (this->textColor == Qt::white) {
        this->setStyleSheet(LEAVE_WHITE_STYLE);
    } else {
        this->setStyleSheet(LEAVE_OTHER_STYLE);
    }
    event->ignore();
}
