#ifndef FFTW3OBJECT_H
#define FFTW3OBJECT_H

#include <QObject>
#include "fftw3.h"
#include "waveformgenerator.h"
#include "cxxlog.h"

class fftw3Object : public QObject {
    Q_OBJECT

public:

    explicit fftw3Object(QObject *parent = nullptr);
    ~fftw3Object();

    enum signaType {
        UINT8,
        INT8,
        UINT16,
        INT16,
        UINT32,
        INT32,
        FLOAT
    };

    // 生成信号
    static void generateSignalData(QByteArray & data,               // 目标数据
                                   WaveformType sigleType,          // 信号类型
                                   int          dataCount = 2048,   // 目标数据个数
                                   int          frequency = 440,    // 频率
                                   double       magnitude = 0.5,    // 幅度[0,1]
                                   double       firstPhase = 0.0,   // 初相[0,2π]
                                   int          channalCount = 2,   // 通道数
                                   int          sampleRate = 44100, // 每秒包含的点数
                                   signaType    bitType = INT16,    // 数据类型
                                   bool         isLittle = true     // 数据端序
                                   );

    template<typename SampleType>
    static void generateSignalData(QVector<SampleType>& data,              // 目标数据
                                   WaveformType         sigleType,         // 信号类型
                                   int                  dataCount = 2048,  // 目标数据个数
                                   int                  frequency = 440,   // 频率
                                   double               magnitude = 0.5,   // 幅度[0,1]
                                   double               firstPhase = 0.0,  // 初相[0,2π]
                                   int                  channalCount = 2,  // 通道数
                                   int                  sampleRate = 44100 // 每秒包含的点数
                                   ) {
        data.clear();
        try {
            WaveformGenerator<SampleType> eGen(sampleRate,
                                               frequency,
                                               magnitude,
                                               firstPhase,
                                               channalCount,
                                               sigleType);

            for (int i = 0; i < dataCount; i++) {
                // data.append(eGen.nextFrame());
                std::vector<SampleType> vec = eGen.nextFrame();

                for (int j = 0; j < vec.size(); j++) {
                    data.append(vec[j]);
                }
            }

            // data.append(eGen.generateBlock(dataCount));
        } catch (const std::exception& e) {
            FATAL_LOG_CXX("Error: %s\n", e.what());
            return;
        }
    }

    static bool fft(const QVector<double>& sdata,
                    QVector<double>      & fftdata,
                    QVector<double>      & radiandata,
                    int                    channalCount = 2,
                    int                    windowSize = 2048,
                    int                    overlap = 1024);

    static bool fft(const QByteArray& sdata,
                    QVector<double> & dsdata,
                    QVector<double> & fftdata,
                    QVector<double> & radiandata,
                    int               channalCount = 2,
                    signaType         bitType = INT16,
                    int               windowSize = 2048,
                    int               overlap = 1024,
                    bool              is_little = true);

    // 开始流式变换 windowSize为窗口大小(每次处理的大小)
    bool fftStreamStart(int windowSize = 1024,
                        int overlap = 512);

    // 加入数据变换
    // 注意:输入数据需要归一化到[-1,1]，比如int16需要除以32767
    // 返回剩余输入个数或-1
    int fftAddStream(const QVector<double>& in,
                     QVector<double>& out,
                     QVector<double>& radiandata);

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
