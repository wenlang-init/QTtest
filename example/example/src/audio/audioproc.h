#ifndef AUDIOPROC_H
#define AUDIOPROC_H

#include <QObject>
#include <QAudioFormat>
#include "fft/fft.h"

class AudioProc : public QObject
{
    Q_OBJECT
public:
    explicit AudioProc(QObject *parent = nullptr);
    ~AudioProc();
    void sqrt(CurveData &frequencyDomainData);
    void setvalue(int fs,double Avalue);
protected:
    void processData(QByteArray &data,const QAudioFormat &format);

public slots:
    void readData(QByteArray data,QAudioFormat format);
    void clearData();
signals:
    void dataflush(CurveData timeData,CurveData frequencyDomainData,QString str);
private:
    QByteArray m_data;
    int m_fs;
    double m_Avalue;
};

#endif // AUDIOPROC_H
