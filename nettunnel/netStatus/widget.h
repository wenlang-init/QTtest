#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "netinfo.h"
#include "public/funchelper.h"
#include "waitdialog.h"
#include <QThread>
#include "lognone.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class FTOBJECT : public QObject {
    Q_OBJECT

public:

    FTOBJECT(QObject *parent = nullptr) : QObject(parent) {
        connect(&FuncHelper::getInstance(), &FuncHelper::sig_md5info, this,
                [ = ](int value) {
            emit sig_info(value);
        });
    }

    ~FTOBJECT() {}

public slots:

    void slot_getmd5(QString file) {
        QString md5 = FuncHelper::getInstance().getFileMD5(file);
        emit    sig_md5(md5);
    }

signals:

    void sig_info(int value);
    void sig_md5(QString md5);
};

class Widget : public QWidget {
    Q_OBJECT

public:

    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:

    void on_pushButton_clicked();

signals:

    void sig_getmd5(QString file);

private:

    Ui::Widget *ui;
    NetInfo *netInfo;
    WaitDialog *waitdialog = nullptr;
    FTOBJECT *ftobject;
    QThread *thread;
};

#endif // WIDGET_H
