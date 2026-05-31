#ifndef FFTW3OBJECT_H
#define FFTW3OBJECT_H

#include <QObject>
#include "fftw3.h"

class fftw3Object : public QObject {
    Q_OBJECT

public:

    explicit fftw3Object(QObject *parent = nullptr);
    ~fftw3Object();
    static bool fft(const QByteArray& sdata,
                    QVector<double> & dsdata,
                    QVector<double> & fftdata,
                    int               channalCount = 2,
                    int               byteRate = 2,
                    int               windowSize = 2048,
                    int               overlap = 1024);

    // 开始流式变换 windowSize为窗口大小(每次处理的大小)
    bool fftStreamStart(int windowSize = 1024,
                        int overlap = 512);

    // 加入数据变换
    // 注意:输入数据需要归一化到[-1,1]，比如int16需要除以32767
    // 返回剩余输入个数或-1
    int fftAddStream(const QVector<double>& in,
                     QVector<double>& out);

    // 结束变换
    void fftStreamSop();

signals:

private:

    int m_windowSize = 1024; // 窗口大小
    int m_overlap = 512;     // 重叠大小
    double *m_window = nullptr;
    double *m_input = nullptr;
    fftw_complex *m_output = nullptr;
    fftw_plan m_plan = nullptr;
    QVector<double>m_overlapVec;
};

#endif // FFTW3OBJECT_H
