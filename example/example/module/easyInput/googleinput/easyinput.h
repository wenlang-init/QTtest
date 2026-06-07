
#ifndef EASYINPUT_H
#define EASYINPUT_H

#include <QObject>
#include <QStringList>
//#include <QVariantList>

class easyInput : public QObject
{
    Q_OBJECT
public:
    explicit easyInput(QObject *parent = 0);
    ~easyInput();

    // 根据拼音查询汉字，返回结果数。pystr:拼音;outstr:结果
    int get_chinese_value(QString &pystr,QStringList &outstr);
    int get_last_count(); // 获取最近一次的结果数
    int get_chinese_s(QStringList &pinyinlist,QList<QStringList> &outstrlist); // 获取多个拼音数据

    void reset_search();
    bool isopen();

    //Q_INVOKABLE QVariantList set_input_value(QString value);
    //Q_INVOKABLE QVariantList get_all_icodedata();

protected:

private:
    bool open(QString &dir);
    int get_chinese(QString &pinyin,QStringList &outstr);

private:
    //所有拼音链表
    QList<QString> allPY;
    //当前拼音索引
    int currentPYIndex;
    //当前拼音数量
    int currentPYCount;
    bool ok;

    // 最大输入长度(拼写字符串长度)
    int maxInputLen;
    // 最大输出长度(汉字字符串长度)
    int maxOutputLen;
};

#endif // EASYINPUT_H
