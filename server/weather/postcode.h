#ifndef POSTCODE_H
#define POSTCODE_H

#include <QObject>

struct PostCodeInfo
{
    QString province; // 省
    QString provinceAbbrevate; // 省简称
    QString region; // 具体地区
    unsigned short regionCode; // 地区代码
    unsigned int postCode; // 邮编
};

class PostCode : public QObject
{
    Q_OBJECT
public:
    static PostCode &getInstance(){
        static PostCode postcode;
        return postcode;
    }
protected:
    explicit PostCode(QObject *parent = nullptr);
    bool init();
public:
    const QList<PostCodeInfo> &getALLPostCode();
    bool getPostCodeFromRegion(PostCodeInfo &pcode,const QString &region);
    bool getPostCodeFromRegionCode(PostCodeInfo &pcode,const unsigned short &regionCode);
    bool getPostCodeFromPostCode(PostCodeInfo &pcode,const unsigned int &postCode);
signals:
private:
    QList<PostCodeInfo> m_data;
};

#endif // POSTCODE_H
