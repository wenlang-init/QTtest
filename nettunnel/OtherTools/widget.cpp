#include "widget.h"
#include "ui_widget.h"
#include "publicip/publicip.h"
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include "weatherwidget.h"
#include "shouyin/shouyinw.h"
#include "delegate/mylistview.h"

#if defined(Q_OS_WIN)
# include "wordtopdf/wordtopdf.h"
#endif // if defined(Q_OS_WIN)

#include "codeeditor/codeedittestw.h"

#include "graphics/graphicswidget.h"

#include "listw/listw.h"
#include "draw/drawwidget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    WeatherWidget *weatherwidget = new WeatherWidget(ui->widget);
    QHBoxLayout   *hlayout = new QHBoxLayout(ui->widget);
    hlayout->addWidget(weatherwidget);

    connect(&PublicIP::getInstance(), &PublicIP::queryFinish, this,
            [this](IpAddressItem& data, bool issuccess) {
        qDebug() << issuccess << data.ipv4 << data.ipv6;
        ui->lineEdit->clear();
        QHostAddress addr;
        addr.setAddress(data.ipv4);
        addr.setAddress(data.ipv6);

        if (issuccess) {
            // ui->lineEdit->setText(data.ipv4);
            ui->lineEdit->setText(addr.toString());
        }
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    PublicIP::getInstance().requestPubliceIP();
}

void Widget::on_pushButton_2_clicked()
{
#if defined(Q_OS_WIN)
    WordToPdf *wtp = new WordToPdf;
    wtp->show();
    connect(wtp, &QWidget::destroyed, this, [ = ]() {
        qDebug();
    });
#endif // if defined(Q_OS_WIN)
}

void Widget::on_pushButton_3_clicked()
{
    ShouYinW *syw = new ShouYinW;

    syw->show();
    syw->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(syw, &QWidget::destroyed, this, [ = ]() {
        qDebug() << sender();
    });
}

void Widget::on_pushButtonqml_clicked()
{
    codeEditTestW *cew = new codeEditTestW;

    cew->setGeometry(pos().rx(), pos().ry(), this->width(), this->height());
    cew->show();
    cew->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(cew, &codeEditTestW::destroyed, this, [ = ]() {
        qDebug() << sender();
    });
}

void Widget::on_pushButtonqmllist_clicked()
{
    ListW *listw = new ListW;

    listw->setGeometry(pos().rx(), pos().ry(), this->width(), this->height());
    listw->show();
    listw->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(listw, &ListW::destroyed, this, [ = ]() {
        qDebug() << sender();
    });
}

void Widget::on_pushButtongraphics_clicked()
{
    GraphicsWidget *ghw = new GraphicsWidget;

    ghw->setGeometry(pos().rx(), pos().ry(), this->width(), this->height());
    ghw->show();
    ghw->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(ghw, &GraphicsWidget::destroyed, this, [ = ]() {
        qDebug() << sender();
    });
}

void Widget::on_pushButton_4_clicked()
{
    MyListView *mv = new MyListView;

    mv->setGeometry(pos().rx(), pos().ry(), this->width(), this->height());
    mv->show();
    mv->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(mv, &GraphicsWidget::destroyed, this, [ = ]() {
        qDebug() << sender();
    });
}

#include "lognone.h"
void Widget::on_pushButton_5_clicked()
{
    drawWidget *dw = new drawWidget;

    dw->setGeometry(pos().rx(), pos().ry(), this->width(), this->height());
    dw->show();
    dw->setAttribute(Qt::WA_DeleteOnClose, true);
    connect(dw, &drawWidget::destroyed, this, [ = ]() {
        qDebug() << sender();
        DEBUG_LOG_RAW("11111111111111111\n");
    });
}
