#include "number_input_widget.h"
#include <QPushButton>
#include <QPainter>
#include <QMouseEvent>

Number_Input_Widget::Number_Input_Widget(QWidget *parent) : QWidget(parent)
{
    background_color = QColor(255,255,255,100);

    QPalette pl;
    pl.setBrush(QPalette::Base,QBrush(QColor(255,255,255,100)));
    QFont font("Microsoft Yahei", 8, QFont::Normal);
    // 按键
    for(unsigned int i=0;i<(sizeof(pushbutton)/sizeof(pushbutton[0]));i++){
        pushbutton[i] = new QPushButton(this);
        pushbutton[i]->installEventFilter(this);
        // 按钮透明
        pushbutton[i]->setFlat(true);
        pushbutton[i]->setFont(font);
        //pushbutton[i]->setPalette(pl);
        // 背景色 圆角
        //pushbutton[i]->setStyleSheet("QPushButton{background-color:rgba(255,255,255,255);border-radius: 10px;}");
        // 字颜色
        //pushbutton[i]->setStyleSheet("QPushButton{color:rgba(0,0,0,255);}");
        //pushbutton[i]->setFont(QFont("宋体",12));
    }
    pushbutton[0]->setText(QLatin1String("0"));
    pushbutton[1]->setText(QLatin1String("1"));
    pushbutton[2]->setText(QLatin1String("2"));
    pushbutton[3]->setText(QLatin1String("3"));
    pushbutton[4]->setText(QLatin1String("4"));
    pushbutton[5]->setText(QLatin1String("5"));
    pushbutton[6]->setText(QLatin1String("6"));
    pushbutton[7]->setText(QLatin1String("7"));
    pushbutton[8]->setText(QLatin1String("8"));
    pushbutton[9]->setText(QLatin1String("9"));
    pushbutton[10]->setText(QLatin1String("."));
    pushbutton[11]->setText(QLatin1String(" "));
    pushbutton[12]->setText(QLatin1String("+"));
    pushbutton[13]->setText(QLatin1String("-"));
    pushbutton[14]->setText(QLatin1String("*"));
    pushbutton[15]->setText(QLatin1String("/"));
    pushbutton[16]->setText(QLatin1String("="));
}

void Number_Input_Widget::reset()
{

}

void Number_Input_Widget::resizeEvent(QResizeEvent *event)
{
    int w = this->width()/4;
    int h1 = this->height()/4;
    int h2 = this->height()/5;

    pushbutton[1]->setGeometry(w*0,0,w,h1);
    pushbutton[2]->setGeometry(w*1,0,w,h1);
    pushbutton[3]->setGeometry(w*2,0,w,h1);
    pushbutton[4]->setGeometry(w*0,h1,w,h1);
    pushbutton[5]->setGeometry(w*1,h1,w,h1);
    pushbutton[6]->setGeometry(w*2,h1,w,h1);
    pushbutton[7]->setGeometry(w*0,h1*2,w,h1);
    pushbutton[8]->setGeometry(w*1,h1*2,w,h1);
    pushbutton[9]->setGeometry(w*2,h1*2,w,h1);
    pushbutton[0]->setGeometry(w*0,h1*3,w,h1);
    pushbutton[10]->setGeometry(w*1,h1*3,w,h1);
    pushbutton[11]->setGeometry(w*2,h1*3,w,h1);

    pushbutton[12]->setGeometry(w*3,0,w,h2);
    pushbutton[13]->setGeometry(w*3,h2,w,h2);
    pushbutton[14]->setGeometry(w*3,h2*2,w,h2);
    pushbutton[15]->setGeometry(w*3,h2*3,w,h2);
    pushbutton[16]->setGeometry(w*3,h2*4,w,h2);

    QWidget::resizeEvent(event);
}

bool Number_Input_Widget::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress){
        QMouseEvent *mouseEvent=static_cast<QMouseEvent *>(event);
        if(mouseEvent->buttons() & Qt::LeftButton){
            for(unsigned int i=0;i<(sizeof(pushbutton)/sizeof(pushbutton[0]));i++){
                if(obj == pushbutton[i]){
                    emit clicked_chinese(pushbutton[i]->text());
                    break;
                }
            }
        }
    }

    return QWidget::eventFilter(obj,event);
}
