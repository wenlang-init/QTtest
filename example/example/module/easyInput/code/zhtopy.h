#ifndef ZHTOPY_H
#define ZHTOPY_H

#include <QObject>
#include <QStringList>

class ZhToPy : public QObject
{
    Q_OBJECT
public:
    static ZhToPy* getManager();
    static void Destroy();

    explicit ZhToPy(QObject *parent = NULL);
    QString zh_to_py(const QString &chinese); // 汉字到拼音
    QString zh_to_jp(const QString &chinese); // 汉字到拼音头

    // 获取字符串每个汉字拼音首字母
    QString get_chinese_pinyin_head_gb2312(QString node);
private:
    void init_jplist();

    char convert_gb2312(quint16 n); // gb2312汉字拼音查询函数
    QString qpinyin_head; // 保存拼音首字母

    bool file_openstauts;
    QStringList listPY;
    QStringList listJP;

    static ZhToPy* m_Manager;
signals:

public slots:
};

#endif // ZHTOPY_H
