#include "shouyinw.h"
#include "ui_shouyinw.h"
#include "decode/beastSound.h"
#include "lognone.h"
#include <QMessageBox>
#include "decode/autf8.h"

ShouYinW::ShouYinW(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShouYinW)
    , checkindex(1)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
}

ShouYinW::~ShouYinW()
{
    delete ui;
    DEBUG_LOG("\n");
}

void ShouYinW::on_pushButton1_clicked()
{
    QString qsdict = ui->lineEdit->text();

    // for(int i=0;i<qsdict.length();i++){
    //     QChar high = qsdict[i];
    //     qDebug()<<"11111111111" << high;
    //     // 字符❤️(\u2764\ufe0f组合字符)解析出问题
    //     if (high.isHighSurrogate() && i + 1 < qsdict.length()) {
    //         QChar low = qsdict[i + 1];
    //         qDebug()<<"222222222222" << high << low;
    //         if (low.isLowSurrogate()) {
    //             uint32_t codePoint = QChar::surrogateToUcs4(high, low);
    //             // 处理码点
    //             i++; // 跳过低代理
    //             qDebug()<<"333333333333" << high << low << QString::number(codePoint,16);
    //             continue;
    //         }
    //     }
    // }
    // 字符❤️(\u2764\ufe0f组合字符)解析出问题
    auto ucs4 = qsdict.toUcs4(); // 字符的Unicode如果超过16bit,QString.size 返回2 ，根据这个函数可得到正确个数
    if(ucs4.size() != 4){
        QMessageBox::information(this,"提示",QString("秘钥为4个不同的字(注:不能是❤️这样的组合字符),%1").arg(qsdict));
        qDebug()<<ucs4<<ucs4.size()<<qsdict<<qsdict.size();
        return;
    }
    for(int i=0;i<ucs4.size();i++){
        for(int j=i+1;j<ucs4.size();j++){
            if(ucs4.at(i) == ucs4.at(j)){
                QMessageBox::information(this,"提示",QString("秘钥为4个不同的字(注:不能是❤️这样的组合字符),%1").arg(qsdict));
                qDebug()<<ucs4<<ucs4.size()<<qsdict<<qsdict.size();
                return;
            }
        }
    }
    QByteArray dictqb[4];
    const char *dict[4];
    for(int i=0;i<4;i++){
#if 0
        dictqb[i] = QString::fromUcs4((const uint*)&ucs4[i]).toUtf8(); // QChar::fromUcs4(ucs4[i]);
#else
        char utf8[7];
        int cnt = UTF8_putc((unsigned char*)utf8,6,ucs4[i]);
        if(cnt < 1){
            QMessageBox::information(this,"提示",QString("秘钥错误,%1").arg(qsdict));
            return;
        }
        dictqb[i].append(utf8,cnt);
        dictqb[i].append("\0");
#endif
        dict[i] = dictqb[i].constData();
        //DEBUG_PRINT_LOG("%s\n",dict[i]);
        //DEBUG_PRINT_LOG("%s\n",utf8);
        //qDebug()<<QString::fromUtf8(utf8);
    }
    QByteArray indata = ui->textEditIn->toPlainText().toUtf8();
    if(indata.size() < 1)return;
    switch (checkindex) {
    case 1:
    {
        char *p = toBeastSound_2byte((const unsigned char *)indata.constData(),(int)indata.size(),dict);
        if(p){
            QString dest;
            dest.append(p);
            free(p);
            ui->textEditOut->setPlainText(dest);
        } else {
            QMessageBox::information(this,"提示","编码错误");
        }
    }
        break;
    case 2:
    {
        char *p = toBeastSound_4byte((const unsigned char *)indata.constData(),(int)indata.size(),dict);
        if(p){
            QString dest;
            dest.append(p);
            free(p);
            ui->textEditOut->setPlainText(dest);
        } else {
            QMessageBox::information(this,"提示","编码错误");
        }
    }
        break;
    case 3:
    {
        char *p = toBeastSound((const char *)indata.constData(),(int)indata.size(),dict);
        if(p){
            QString dest;
            dest.append(p);
            free(p);
            ui->textEditOut->setPlainText(dest);
        } else {
            QMessageBox::information(this,"提示","编码错误");
        }
    }
        break;
    default:
        break;
    }
}


void ShouYinW::on_pushButton2_clicked()
{
    QByteArray indata = ui->textEditOut->toPlainText().toUtf8();
    if(indata.size() < 1)return;
    switch (checkindex) {
    case 1:
    {
        char *p = fromBeastSound_2byte((const unsigned char *)indata.constData(),(int)indata.size());
        if(p){
            QString dest;
            dest.append(p);
            free(p);
            ui->textEditIn->setPlainText(dest);
        } else {
            QMessageBox::information(this,"提示","解码错误");
        }
    }
    break;
    case 2:
    {
        char *p = fromBeastSound_4byte((const unsigned char *)indata.constData(),(int)indata.size());
        if(p){
            QString dest;
            dest.append(p);
            free(p);
            ui->textEditIn->setPlainText(dest);
        } else {
            QMessageBox::information(this,"提示","解码错误");
        }
    }
    break;
    case 3:
    {
        int destsize;
        char *p = fromBeastSound((const char *)indata.constData(),(int)indata.size(),&destsize);
        if(p){
            QByteArray destb;
            destb.append(p,destsize);
            free(p);
            QString dest = destb;
            ui->textEditIn->setPlainText(dest);
        } else {
            QMessageBox::information(this,"提示","解码错误");
        }
    }
    break;
    default:
        break;
    }
}


void ShouYinW::on_checkBox1_clicked()
{
    checkindex = 1;
    ui->checkBox1->setChecked(true);
    ui->checkBox1_2->setChecked(false);
    ui->checkBox3->setChecked(false);
}



void ShouYinW::on_checkBox1_2_clicked()
{
    checkindex = 2;
    ui->checkBox1->setChecked(false);
    ui->checkBox1_2->setChecked(true);
    ui->checkBox3->setChecked(false);
}


void ShouYinW::on_checkBox3_clicked()
{
    checkindex = 3;
    ui->checkBox1->setChecked(false);
    ui->checkBox1_2->setChecked(false);
    ui->checkBox3->setChecked(true);
}

