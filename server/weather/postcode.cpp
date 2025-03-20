#include "postcode.h"
#include <QFile>
#include <QDebug>
#define qdebug qDebug().noquote()<<__FILE__<<__LINE__<<__FUNCTION__

PostCode::PostCode(QObject *parent)
    : QObject{parent}
{
    init();
}

bool PostCode::init()
{
    QFile file(":/weather/citydata.txt");

    if(false == file.open(QIODevice::ReadOnly)){
        qdebug << file.errorString();
        return false;
    }
    //file.readLine();
    QByteArray data = file.readAll();
    file.close();
    if(data.isEmpty()){
        qdebug << "not found data:" << file.errorString();
        return false;
    }

    QStringList strlist = QString(data).split('\n');
    for(int i=0;i<strlist.size();i++){
        QStringList list = strlist[i].split('\t');
        if(list.size() != 5){
            qdebug << "data error";
            return false;
        }
        PostCodeInfo node;
        node.province = list[0];
        node.provinceAbbrevate = list[1];
        node.region = list[2];
        node.regionCode = list[3].toUShort();
        node.postCode = list[4].toUInt();
        m_data.append(node);
    }

    return true;
}

const QList<PostCodeInfo> &PostCode::getALLPostCode()
{
    return m_data;
}

bool PostCode::getPostCodeFromRegion(PostCodeInfo &pcode,const QString &region)
{
    for(int i=0;i<m_data.size();i++){
        if(m_data[i].region == region){
            pcode = m_data[i];
            return true;
        }
    }
    return false;
}

bool PostCode::getPostCodeFromRegionCode(PostCodeInfo &pcode,const unsigned short &regionCode)
{
    for(int i=0;i<m_data.size();i++){
        if(m_data[i].regionCode == regionCode){
            pcode = m_data[i];
            return true;
        }
    }
    return false;
}

bool PostCode::getPostCodeFromPostCode(PostCodeInfo &pcode,const unsigned int &postCode)
{
    for(int i=0;i<m_data.size();i++){
        if(m_data[i].postCode == postCode){
            pcode = m_data[i];
            return true;
        }
    }
    return false;
}
