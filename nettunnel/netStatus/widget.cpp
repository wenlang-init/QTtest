#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QStandardPaths>
#include "lognone.h"
#include <QThread>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    netInfo = new NetInfo(this);
    connect(netInfo, &NetInfo::sig_netInfo, this, [ = ](QString& rdata) {
        ui->label->setText(rdata);
    });
    ftobject = new FTOBJECT;
    thread = new QThread(this);
    ftobject->moveToThread(thread);
    thread->start();
    connect(ftobject, &FTOBJECT::sig_info, this, [&](int value) {
        waitdialog->set_text(QString::number(value) + "%");
        waitdialog->set_progress(value);

        // DEBUG_LOG("%d%%\n", value);
    });
    connect(ftobject, &FTOBJECT::sig_md5, this, [&](QString md5) {
        ui->label3->setText(md5);
        DEBUG_LOG("%s\n", md5.toLocal8Bit().constData());
        delete waitdialog;
        waitdialog = nullptr;
    });
    connect(this, &Widget::sig_getmd5, ftobject, &FTOBJECT::slot_getmd5);
}

Widget::~Widget()
{
    thread->quit();
    thread->wait();
    delete thread;

    if (waitdialog) {
        delete waitdialog;
    }
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    if (waitdialog) {
        return;
    }
    QFileDialog fileDialog(this);

    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setWindowTitle(tr("打开文件"));
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::
                                                              MoviesLocation)
                            .value(0, QDir::homePath()));

    if (fileDialog.exec() == QDialog::Accepted) {
        // const QUrl url = fileDialog.selectedUrls().constFirst();
        QString file = fileDialog.selectedFiles().constFirst();
        ui->label2->setText(file);

        // DEBUG_LOG("%s\n", file.toStdString().c_str());
        DEBUG_LOG("%s\n", file.toLocal8Bit().constData());

        // QString md5 = FuncHelper::getInstance().getFileMD5(file);
        waitdialog = new WaitDialog(this);

        // 点击取消
        connect(waitdialog, &WaitDialog::cancel_waiting_signal, this, [&]() {});
        waitdialog->setWindowModality(Qt::WindowModal);
        waitdialog->set_btn_cancel_show(false);
        waitdialog->set_btn_cancel(true);
        waitdialog->set_text(tr("0%"));
        waitdialog->move_to_center(this);

        // waitdialog->set_progress(50);
        waitdialog->show();

        emit sig_getmd5(file);
    }
}
