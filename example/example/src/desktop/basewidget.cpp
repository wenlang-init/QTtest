#include "basewidget.h"

BaseWidget::BaseWidget(QWidget *parent)
    : QWidget(parent)
    , widgetIndex(-1)
{
    // QPalette pe;
    // pe.setColor(QPalette::WindowText,QColor(Qt::black));
    // QFont font("Microsoft Yahei", 8, QFont::Normal);

    input_status = false;

    pe.setColor(QPalette::WindowText, QColor(Qt::black));
    pe.setBrush(QPalette::Base, QBrush(QColor(255, 255, 255, 0)));
    font.setPointSize(8);
    font.setFamily(QString::fromUtf8("Microsoft Yahei"));
    this->setPalette(pe);
    this->setFont(font);

    // 连接按键信号槽
    // connect(this,SIGNAL(Key_Pass_Signal(int,int)),this,SLOT(Key_Pass_Slot(int,int)));
}

void BaseWidget::setWidgetIndex(int index)
{
    widgetIndex = index;
}

int BaseWidget::getWidgetIndex()
{
    return widgetIndex;
}

void BaseWidget::set_input_status(bool status)
{
    input_status = status;
}

void BaseWidget::returnWidget()
{
    if (widgetIndex >= 0)
    {
        emit returnSignal(widgetIndex);
    }
}
