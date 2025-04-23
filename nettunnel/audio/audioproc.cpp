#include "audioproc.h"
#include <QDebug>

AudioProc::AudioProc(QObject *parent)
    : QObject{parent}
{}

AudioProc::~AudioProc()
{
}

void AudioProc::sqrt(CurveData &frequencyDomainData)
{
    int left = 0;
    int right = frequencyDomainData.valVec.size() - 1;
    int lastSwapIndex = 0;      // 记录最后一次交换的位置
    bool hasSwap = false;    // 标志位
    double temp;
    while (left < right) {
        for (int i = left; i < right; i++) { // 保证 a[right] 是最大的
            if (frequencyDomainData.valVec[i] > frequencyDomainData.valVec[i+1]) {
                temp = frequencyDomainData.valVec[i];
                frequencyDomainData.valVec[i] = frequencyDomainData.valVec[i+1];
                frequencyDomainData.valVec[i+1] = temp;

                temp = frequencyDomainData.keyVec[i];
                frequencyDomainData.keyVec[i] = frequencyDomainData.keyVec[i+1];
                frequencyDomainData.keyVec[i+1] = temp;

                hasSwap = true;
                lastSwapIndex = i;
            }
        }
        right = lastSwapIndex;  // 将最后一次交换的位置作为右边界
        if (!hasSwap) { // 上一轮没有交换，提前结束
            break;
        }
        hasSwap = false;
        for (int i = right; i > left; i--) { // 保证 a[left] 是最小的
            if (frequencyDomainData.valVec[i] < frequencyDomainData.valVec[i-1]) {
                temp = frequencyDomainData.valVec[i];
                frequencyDomainData.valVec[i] = frequencyDomainData.valVec[i-1];
                frequencyDomainData.valVec[i-1] = temp;

                temp = frequencyDomainData.keyVec[i];
                frequencyDomainData.keyVec[i] = frequencyDomainData.keyVec[i-1];
                frequencyDomainData.keyVec[i-1] = temp;

                hasSwap = true;
                lastSwapIndex = i;
            }
        }
        left = lastSwapIndex;  // 将最后一次交换的位置作为左边界
        if (!hasSwap) { // 上一轮没有交换，提前结束
            break;
        }
        hasSwap = false;
    }
}

void AudioProc::setvalue(int fs, double Avalue)
{
    m_fs = fs;
    m_Avalue = Avalue;
}

void AudioProc::processData(QByteArray &data,const QAudioFormat &format)
{
    CurveData timeData;
    CurveData frequencyDomainData;

    int index = 0;
    switch(format.sampleFormat()){
    case QAudioFormat::UInt8:
        for(qsizetype i=0;i<data.size();i++){
            timeData.keyVec.append(index);
            timeData.valVec.append((unsigned char)data[i]);
        }
        break;
    case QAudioFormat::Int16:
        for(qsizetype i=0;i<data.size();i+=2){
            timeData.keyVec.append(index);
            timeData.valVec.append(*(qint16*)&data[i]);
        }
        break;
    case QAudioFormat::Int32:
        for(qsizetype i=0;i<data.size();i+=4){
            timeData.keyVec.append(index);
            timeData.valVec.append(*(qint32*)&data[i]);
        }
        break;
    case QAudioFormat::Float:
        for(qsizetype i=0;i<data.size();i+=4){
            timeData.keyVec.append(index);
            timeData.valVec.append(*(float*)&data[i]);
        }
        break;
    default:
        break;
    }

    fft::timeDomainToFreDomain(timeData,frequencyDomainData,m_fs);

#if 1
    struct node{
        double key;
        double val;
        qsizetype index;
    };
#define MAXPOINTCNT 10
    int m=0;
    node max[MAXPOINTCNT];
    max[m].key = 0;
    max[m].val = 0;
    max[m].index = 0;
    for(int i=0;i<frequencyDomainData.keyVec.size();i++){
        if(max[m].val < frequencyDomainData.valVec[i]){
            max[m].key = frequencyDomainData.keyVec[i];
            max[m].val = frequencyDomainData.valVec[i];
            max[m].index = i;
        }
    }
    m++;
    while(1){
        max[m].key = 0;
        max[m].val = 0;
        bool isnew = false;
        for(int i=0;i<frequencyDomainData.keyVec.size();i++){
            if(max[m].val < frequencyDomainData.valVec[i]){
                if(frequencyDomainData.valVec[i]<=max[m-1].val && i!=max[m-1].index){
                    max[m].key = frequencyDomainData.keyVec[i];
                    max[m].val = frequencyDomainData.valVec[i];
                    max[m].index = i;
                    isnew = true;
                }
            }
        }
        if(isnew == false)break;
        // 放弃小数据
        if(max[m].val < m_Avalue){
            break;
        }
        m++;
        if(m >= MAXPOINTCNT)break;
    }
    node tmp{0,0};
    for(int i=0;i<m-1;i++){
        for(int j=i;j<m;j++){
            if(max[i].key > max[j].key){
                tmp = max[j];
                max[j] = max[i];
                max[i] = tmp;
            }
        }
    }

    QString str1="幅值:";
    QString str2="归一化比例:";
    QString str3="频率(HZ):";
    for(int i=0;i<m;i++){
        str1 += " " + QString::number(max[i].val,'g',6);
        str3 += " " + QString::number(max[i].key*qPow(2,m_fs),'g',6);
        str2 += " " + QString::number(max[i].key,'g',6);
    }
    QString str = str1 + "\n" + str2 + "\n" + str3;
#else
    struct node{
        double key;
        double val;
    };

    node max{0,0},max1{0,0},max2{0,0};

    for(int i=0;i<frequencyDomainData.keyVec.size();i++){
        if(max.val < frequencyDomainData.valVec[i]){
            max.key = frequencyDomainData.keyVec[i];
            max.val = frequencyDomainData.valVec[i];
        }
    }

    for(int i=0;i<frequencyDomainData.keyVec.size();i++){
        if(max1.val < frequencyDomainData.valVec[i] && frequencyDomainData.valVec[i]<max.val){
            max1.key = frequencyDomainData.keyVec[i];
            max1.val = frequencyDomainData.valVec[i];
        }
    }
    for(int i=0;i<frequencyDomainData.keyVec.size();i++){
        if(max2.val < frequencyDomainData.valVec[i] && frequencyDomainData.valVec[i]<max1.val){
            max2.key = frequencyDomainData.keyVec[i];
            max2.val = frequencyDomainData.valVec[i];
        }
    }

    QString str1="归一化数据:";
    QString str2="点个数:";
    CurveData frequencyDomainDatatmp = frequencyDomainData;
    sqrt(frequencyDomainDatatmp);
    for(int i=frequencyDomainDatatmp.keyVec.size()-1;i>=0;i--){
        if(i < frequencyDomainDatatmp.keyVec.size()-8)break;
        str1 += "," + QString::number(frequencyDomainDatatmp.keyVec[i],'g',16);
        if(frequencyDomainDatatmp.keyVec[i] != 0)str2 += "," + QString::number(2/frequencyDomainDatatmp.keyVec[i],'g',16);
    }
    QString str = str1 + "\n" + str2;
#endif
    emit dataflush(timeData,frequencyDomainData,str);
}

void AudioProc::readData(QByteArray data, QAudioFormat format)
{
    m_data.append(data);
    if(m_data.size() > qPow(2,m_fs+1)){
        QByteArray data = m_data.left(qPow(2,m_fs+1));
        m_data.remove(0,qPow(2,m_fs+1));
        processData(data,format);
    }
}

void AudioProc::clearData()
{
    m_data.clear();
}
