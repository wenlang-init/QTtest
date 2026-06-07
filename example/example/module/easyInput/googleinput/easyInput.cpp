#include "easyinput.h"
#include "googlepinyin/pinyinime.h"
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <QApplication>

using namespace ime_pinyin;

easyInput::easyInput(QObject *parent) : QObject(parent)
{
    ok=false;
    maxInputLen=26;
    maxOutputLen=26;
    currentPYCount=0;

    QString appstr;
    appstr = QCoreApplication::applicationDirPath(); // 程序路径
    appstr += "/easyInput";

    open(appstr);
    if(0) {
        QString pinyin="alvshd";QStringList outstr;
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<get_chinese_value(pinyin,outstr);
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<outstr;
    }
}

easyInput::~easyInput()
{
    if(ok){
        im_close_decoder();
    }
}

int easyInput::get_chinese_value(QString &pinyin,QStringList &outstr)
{
    if(!ok){
        return currentPYIndex;
    }
    if(pinyin.length() > maxInputLen){
        pinyin = pinyin.left(maxInputLen);
        //pinyin.remove(maxInputLen,-1);
        return currentPYCount;
    }

    outstr.clear();

    currentPYCount = get_chinese(pinyin,outstr);

    return currentPYCount;
}

int easyInput::get_last_count()
{
    return currentPYCount;
}

int easyInput::get_chinese_s(QStringList &pinyinlist, QList<QStringList> &outstrlist)
{
    if(!ok){
        return 0;
    }
    int allcnt=0;
    for(int i=0;i<pinyinlist.count();i++){
        QStringList outstr;
        QString pinyin = pinyinlist.at(i);
        allcnt += get_chinese(pinyin,outstr);
        outstrlist.append(outstr);
    }
    return allcnt;
}

void easyInput::reset_search()
{
    if (!ok){
        return;
    }
    im_reset_search();
}

bool easyInput::isopen()
{
    return ok;
}

bool easyInput::open(QString &dbPath)
{
    QString pyPath = QString("%1/dict_pinyin.dat").arg(dbPath);
    QString pyUserPath = QString("%1/dict_pinyin_user.dat").arg(dbPath);
    ok = im_open_decoder(pyPath.toUtf8().constData(), pyUserPath.toUtf8().constData());

    if (ok) {
        im_set_max_lens(maxInputLen, maxOutputLen);
        im_reset_search();
    } else {
        qDebug()<<__FILE__<<__LINE__<<__FUNCTION__<<"load GooglePinYin error"<<dbPath;
    }

    return ok;
}

int easyInput::get_chinese(QString &pinyin, QStringList &outstr)
{
    outstr.clear();
    int cnt = 0;
    if(!ok){
        return cnt;
    }
//    if(pinyin.length() > maxInputLen){
//        pinyin = pinyin.left(maxInputLen);
//        pinyin.remove(maxInputLen,-1);
//    }

    size_t single = 0;
    size_t multi = 0;
    char16 *cand_buf = new char16[maxOutputLen];
    QByteArray bytearray = pinyin.toUtf8();
    char *py = bytearray.data();
    size_t count = im_search(py, bytearray.size()); // 增量搜索，相同往后面接着搜索
    for (size_t i = 0; i < count; i++) {
        im_get_candidate(i, cand_buf, maxOutputLen);
        if (strlen((char *)cand_buf) > 2) {
            multi++; // 多字
            //printf("code:%s\n",cand_buf);fflush(stdout);
        } else {
            single++; // 单字
            if (single > 40) {
                //break;
            }
        }
        QString cand_str = QString::fromUtf16(cand_buf);
        if(i == 0){ // 将第一个字符串中已经确认的固定的字符串移除im_choose();
            //im_choose(2);im_cancel_last_choice();
            cand_str.remove(0, im_get_fixed_len());
        }
        outstr.append(cand_str);
    }
    delete cand_buf;
    cnt = multi + single;
    //qDebug()<<__FILE__<<__LINE__<<outstr;
    return cnt;
}
