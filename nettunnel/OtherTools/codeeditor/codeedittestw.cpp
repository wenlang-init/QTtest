#include "codeedittestw.h"
#include "ui_codeedittestw.h"
#include <QFileDialog>
#include <QQmlContext>
#include <QQuickWidget>
#include <QDebug>
#define qdebug qDebug()<<__FILE__<<__LINE__<<__FUNCTION__


codeEditTestW::codeEditTestW(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::codeEditTestW)
{
    ui->setupUi(this);

    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);
    editor = new CodeEditor(this);
    editor->setFont(font);
    editor->setPlainText("import QtQuick\n"
                         "Item {\n"
                         "   Rectangle {\n"
                         "       color:\"#00ff00\"\n"
                         "       anchors.fill: parent\n"
                         "   }\n"
                         "}");

    Highlighter *highlighter = new Highlighter();
    highlighter->setDocument(editor->document());
    //Highlighter *highlighter = new Highlighter(editor->document());
    Q_UNUSED(highlighter)


    textedit = new QTextEdit(ui->widget);
    textedit->setReadOnly(true);
    //QLabel *label = new QLabel(ui->widgetshow);
    QString qss;
    //不斜体、加粗、大小25个像素  字体颜色：#F0F0F0 边框宽度：2像素 实线 边框颜色：#AAAAAA  背景色：#000000
    qss.append("QTextEdit{font: normal bold 25px;color:#F0F0F0;border:2px solid #AAAAAA;background:#000000;}");
    //获得焦点时的样式
    qss.append("QTextEdit:focus{border:2px solid #00BB9E;background:#555555;}");
    //鼠标略过时的样式
    qss.append("QTextEdit:hover{color: red;border-color: green;background-color: aqua;}");
    textedit->setStyleSheet(qss);
    textedit->hide();
    QQuickWidget *qw = new QQuickWidget(ui->widget);
    qw->setResizeMode(QQuickWidget::SizeRootObjectToView);
    //qw->engine()->addImportPath("qrc:/");
    qw->rootContext()->setContextProperty("widget",this);
    //qw->setSource(QUrl("qrc:/qml/loaderwgt.qml"));
    qw->showFullScreen();
    connect(qw,&QQuickWidget::sceneGraphError,this,[=](QQuickWindow::SceneGraphError error, const QString &message){
        qdebug<<error<<message;
    });
    connect(qw,&QQuickWidget::statusChanged,this,[=](QQuickWidget::Status status){
        //qdebug<<status<<qw->errors()<<strlist.size();
        if(status == QQuickWidget::Ready || status == QQuickWidget::Error){
            // 加载完成
            if(strlist.size() > 0){
                QFile::remove(strlist[0]);
                //strlist.remove(0);
                strlist.removeFirst();
            }
            if(status == QQuickWidget::Error){
                const auto &errors = qw->errors();
                QString err;
                for(int i=0;i<errors.size();i++){
                    err.append(errors[i].toString());
                }
                textedit->setText(err);
                textedit->show();
                qw->hide();
            }
        }
    });

    connect(editor,&QPlainTextEdit::textChanged,this,[=](){
        QString name = "test.qml." + QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
        QString filename = QCoreApplication::applicationDirPath() + "/" + name;
        strlist.append(filename);
        QFile file(filename);
        if(file.open(QIODevice::WriteOnly)){
            file.write(editor->toPlainText().toLatin1());
            file.close();
            textedit->hide();
            qw->show();
            qw->setSource(QUrl::fromLocalFile(file.fileName()));
        }
        //set_qmlQString(filename);
    });

    QHBoxLayout *hboxlayouttcode = new QHBoxLayout(ui->widgetedit);
    hboxlayouttcode->addWidget(editor);
    QHBoxLayout *hboxlayouttshow = new QHBoxLayout(ui->widget);
    ui->widget->setMinimumHeight(300);
    hboxlayouttshow->addWidget(qw);
    hboxlayouttshow->addWidget(textedit);
}

codeEditTestW::~codeEditTestW()
{
    delete ui;
}

void codeEditTestW::on_pushButton_clicked()
{
    QFileDialog filedialog(this);
    filedialog.setNameFilter("文件(*.qml)");
    filedialog.setViewMode(QFileDialog::Detail);
    filedialog.setAcceptMode(QFileDialog::AcceptOpen);
    filedialog.setFileMode(QFileDialog::ExistingFile);
    filedialog.exec();
    QStringList filelist = filedialog.selectedFiles();
    qdebug<<filelist;
    if(filelist.size()>0){
        QFile file(filelist[0]);
        if (file.open(QFile::ReadOnly | QFile::Text))
            editor->setPlainText(file.readAll());
    }
}

