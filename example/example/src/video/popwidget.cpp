#include "popwidget.h"
#include <QHBoxLayout>

popWidget::popWidget(QString text, QWidget *parent)
    : QDialog{parent}
{
    setWindowFlags(Qt::FramelessWindowHint);    // 无边框
    setAttribute(Qt::WA_TranslucentBackground); // 背景透明
    // setWindowOpacity(0.9);                      // 透明度
    label = new QLabel(this);
    QHBoxLayout *hlayout = new QHBoxLayout(this);
    hlayout->addWidget(label);
    hlayout->setContentsMargins(0, 0, 0, 0);
    label->setText(text);
    label->setAlignment(Qt::AlignCenter);
    QFont font;
    font.setPointSize(20);
    label->setFont(font);
}

void popWidget::settext(const QString& text)
{
    label->setText(text);
}

void popWidget::setfont(const QFont& font)
{
    label->setFont(font);
}
