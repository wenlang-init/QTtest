#include "wordtopdf.h"
#include "ui_wordtopdf.h"
#include <QAxObject>
#include <QDebug>
#include <QFileDialog>
#include <QThread>
#include "lognone.h"

#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

WTP::WTP(QObject *parent)
    : QObject(parent)
{

}

void WTP::wtpfun(QString srcname, QString dstname)
{
    qdebug;
    if(!QFile::exists(srcname)){
        DEBUG_LOG("not found %s\n",srcname.toLocal8Bit().constData());
        return;
    }
    if(QFile::exists(dstname)){
        QFile::remove(dstname);
    }
    QAxObject *pWordApplication = new QAxObject("Word.Application",0);
    QAxObject *pWordDocuments = pWordApplication->querySubObject("Documents");

    QVariant filename(srcname);
    QVariant confirmconversions(false);
    QVariant readonly(true);
    QVariant addtorecentfiles(false);
    QVariant passworddocument("");
    QVariant passwordtemplate("");
    QVariant revert(false);

    QAxObject *doc = pWordDocuments->querySubObject("Open(const QVariant&, const QVariant&,const QVariant&, "
                                                    "const QVariant&, const QVariant&, "
                                                    "const QVariant&,const QVariant&)",
                                                    filename,
                                                    confirmconversions,
                                                    readonly,
                                                    addtorecentfiles,
                                                    passworddocument,
                                                    passwordtemplate,
                                                    revert);

    QVariant OutputFileName(dstname);
    QVariant ExportFormat(17);  //转换PDF
    QVariant OpenAfterExport(false);

    doc->querySubObject("ExportAsFixedFormat(const QVariant&,const QVariant&,const QVariant&)",
                        OutputFileName,
                        ExportFormat,
                        OpenAfterExport);
    doc->dynamicCall("Close(boolean)",false);
    qdebug;
}

WordToPdf::WordToPdf(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::WordToPdf)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    thread = new QThread;
    wtp = new WTP;
    wtp->moveToThread(thread);
    thread->start();
    connect(this,SIGNAL(wtpfun(QString,QString)),wtp,SLOT(wtpfun(QString,QString)));
}

WordToPdf::~WordToPdf()
{
    DEBUG_LOG("\n");
    delete ui;
    thread->quit();
    DEBUG_LOG("\n");
    thread->wait();
    DEBUG_LOG("\n");
    delete thread;
    delete wtp;
    DEBUG_LOG("\n");
}

void WordToPdf::on_pushButton_clicked()
{
    QFileDialog filedialog(this);
    filedialog.setNameFilter("word文件(*.doc *.docx)");
    filedialog.setViewMode(QFileDialog::Detail);
    filedialog.setAcceptMode(QFileDialog::AcceptOpen);
    filedialog.setFileMode(QFileDialog::ExistingFile);
    filedialog.exec();
    QStringList filelist = filedialog.selectedFiles();
    qDebug()<<filelist;
    if(filelist.size()>0){
        QString srcname,dstname;
        srcname = filelist[0];
        dstname = srcname.split(".").at(0) + ".pdf";
        emit wtpfun(srcname,dstname);
    }
}
