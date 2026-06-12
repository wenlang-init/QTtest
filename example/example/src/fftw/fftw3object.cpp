#include "fftw3object.h"
#include <QtEndian>

fftw3Object::fftw3Object(QObject *parent)
    : QObject{parent}
{}

fftw3Object::~fftw3Object()
{
    fftStreamSop();
}

void fftw3Object::generateSignalData(QByteArray & data,
                                     WaveformType sigleType,
                                     int          dataCount,
                                     int          frequency,
                                     double       magnitude,
                                     double       firstPhase,
                                     int          channalCount,
                                     int          sampleRate,
                                     signaType    bitType,
                                     bool         isLittle)
{
    data.clear();

    switch (bitType) {
    case UINT8: {
        try {
            WaveformGenerator<unsigned char> eGen(sampleRate,
                                                  frequency,
                                                  magnitude,
                                                  firstPhase,
                                                  channalCount,
                                                  sigleType);
            data.append(eGen.generateBlock(dataCount));
        } catch (const std::exception& e) {
            FATAL_LOG_CXX("Error: %s\n", e.what());
            return;
        }
        break;
    }

    case INT8: {
        try {
            WaveformGenerator<char> eGen(sampleRate,
                                         frequency,
                                         magnitude,
                                         firstPhase,
                                         channalCount,
                                         sigleType);
            data.append(eGen.generateBlock(dataCount));
        } catch (const std::exception& e) {
            FATAL_LOG_CXX("Error: %s\n", e.what());
            return;
        }
        break;
    }

    case UINT16: {
        try {
            WaveformGenerator<unsigned short> eGen(sampleRate,
                                                   frequency,
                                                   magnitude,
                                                   firstPhase,
                                                   channalCount,
                                                   sigleType);
            auto gen = eGen.generateBlock(dataCount);

            for (int i = 0; i < gen.size(); i += channalCount) {
                unsigned short v = gen[i];

                if (isLittle) {
                    v = qToLittleEndian(gen[i]);
                } else {
                    v = qToBigEndian(gen[i]);
                }
                data.append((char *)&v, sizeof(v));
            }
        } catch (const std::exception& e) {
            FATAL_LOG_CXX("Error: %s\n", e.what());
            return;
        }
        break;
    }

    case INT16: {
        try {
            WaveformGenerator<short> eGen(sampleRate,
                                          frequency,
                                          magnitude,
                                          firstPhase,
                                          channalCount,
                                          sigleType);
            auto gen = eGen.generateBlock(dataCount);

            for (int i = 0; i < gen.size(); i += channalCount) {
                short v = gen[i];

                if (isLittle) {
                    v = qToLittleEndian(gen[i]);
                } else {
                    v = qToBigEndian(gen[i]);
                }
                data.append((char *)&v, sizeof(v));
            }
        } catch (const std::exception& e) {
            FATAL_LOG_CXX("Error: %s\n", e.what());
            return;
        }
        break;
    }

    case UINT32: {
        try {
            WaveformGenerator<unsigned int> eGen(sampleRate,
                                                 frequency,
                                                 magnitude,
                                                 firstPhase,
                                                 channalCount,
                                                 sigleType);
            auto gen = eGen.generateBlock(dataCount);

            for (int i = 0; i < gen.size(); i += channalCount) {
                unsigned int v = gen[i];

                if (isLittle) {
                    v = qToLittleEndian(gen[i]);
                } else {
                    v = qToBigEndian(gen[i]);
                }
                data.append((char *)&v, sizeof(v));
            }
        } catch (const std::exception& e) {
            FATAL_LOG_CXX("Error: %s\n", e.what());
            return;
        }
        break;
    }

    case INT32: {
        try {
            WaveformGenerator<int> eGen(sampleRate,
                                        frequency,
                                        magnitude,
                                        firstPhase,
                                        channalCount,
                                        sigleType);
            auto gen = eGen.generateBlock(dataCount);

            for (int i = 0; i < gen.size(); i += channalCount) {
                int v = gen[i];

                if (isLittle) {
                    v = qToLittleEndian(gen[i]);
                } else {
                    v = qToBigEndian(gen[i]);
                }
                data.append((char *)&v, sizeof(v));
            }
        } catch (const std::exception& e) {
            FATAL_LOG_CXX("Error: %s\n", e.what());
            return;
        }
        break;
    }

    case FLOAT: {
        try {
            WaveformGenerator<float> eGen(sampleRate,
                                          frequency,
                                          magnitude,
                                          firstPhase,
                                          channalCount,
                                          sigleType);
            auto gen = eGen.generateBlock(dataCount);

            for (int i = 0; i < gen.size(); i++) {
                float v = gen[i];

                if (isLittle) {
                    v = qToLittleEndian(gen[i]);
                } else {
                    v = qToBigEndian(gen[i]);
                }
                data.append((char *)&v, sizeof(v));
            }
        } catch (const std::exception& e) {
            FATAL_LOG_CXX("Error: %s\n", e.what());
            return;
        }
        break;
    }

    default:
        break;
    }
}

bool fftw3Object::fft(const QVector<double>& sdata,
                      QVector<double>      & fftdata,
                      QVector<double>      & radiandata,
                      int                    windowSize,
                      int                    overlap)
{
    const QVector<double>& tmpData = sdata;

    QVector<double> window(windowSize);

    // 汉宁窗
    for (int i = 0; i < windowSize; ++i) {
        window[i] = 0.5 * (1.0 - cos(2.0 * M_PI * i / (windowSize - 1)));
    }
    fftw_plan plan;
    double   *in = fftw_alloc_real(windowSize);

    if (!in) return false;

    fftw_complex *out = fftw_alloc_complex(windowSize / 2 + 1);

    if (!out) {
        fftw_free(in);
        return false;
    }
    plan = fftw_plan_dft_r2c_1d(windowSize, in, out, FFTW_ESTIMATE);

    if (!plan) {
        fftw_free(in);
        fftw_free(out);
        return false;
    }

    int frame_count = 0;
    fftdata.resize(windowSize / 2 + 1, 0.0);
    radiandata.resize(windowSize / 2 + 1, 0.0);

    QVector<double> tmpDataOverlap(overlap, 0);
    const int needCount = windowSize - overlap;

    for (int i = 0; i + needCount <= tmpData.size(); i += needCount) {
        for (int j = 0; j < windowSize; j++) {
            if (j < overlap) {
                in[j] = tmpDataOverlap[j];
            } else {
                in[j] = tmpData[i + j - overlap] * window[j];
                tmpDataOverlap[j - overlap] = in[j];
            }
        }

        fftw_execute(plan);

        for (int j = 0; j <= windowSize / 2; ++j) {
            double mag = sqrt(out[j][0] * out[j][0] + out[j][1] * out[j][1]);

            // 振幅
            fftdata[j] += mag / (windowSize / 2);

            // 如果希望幅值以dBFS表示，可以进行对数转换:
            // double db = 20 * log10((mag / (m_windowSize / 2)) + 1e-12);

            // 相位(弧度) atan2:四象限反正切函数，结果范围在[-π, π]之间
            // double radian = atan2(out[j][1], out[j][0]);
            radiandata[j] += atan2(out[j][1], out[j][0]);

            // 频率 f=j*采样率/采样点数
            // double freq = j * 48000 / windowSize;
        }
        frame_count++;
    }

    // 计算平均值
    if (frame_count > 1) {
        for (auto& m : fftdata) m /= frame_count;

        for (auto& m : radiandata) m /= frame_count;
    }

    // 5. 清理资源
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
    return true;
}

bool fftw3Object::fft(const QByteArray& sdata,
                      QVector<double> & dsdata,
                      QVector<double> & fftdata,
                      QVector<double> & radiandata,
                      int               channalCount,
                      signaType         bitType,
                      int               windowSize,
                      int               overlap,
                      bool              is_little)
{
    QVector<double> tmpData;
    int size = sdata.size();

    dsdata.clear();
    int count;

    switch (bitType) {
    case UINT8: {
        count = size;
        double maxVal_ = std::numeric_limits<quint8>::max();
        const quint8 *p = (quint8 *)sdata.data();

        for (int i = 0; i < count; i += channalCount) {
            double v = p[i];
            tmpData.append(v / maxVal_);
            dsdata.append(v);
        }
        break;
    }

    case INT8: {
        count = size;
        double maxVal_ = std::numeric_limits<qint8>::max();
        const qint8 *p = (qint8 *)sdata.data();

        for (int i = 0; i < count; i += channalCount) {
            double v = p[i];
            tmpData.append(v / maxVal_);
            dsdata.append(v);
        }
        break;
    }

    case UINT16: {
        count = size / 2;
        const quint16 *p = (quint16 *)sdata.data();
        double maxVal_ = std::numeric_limits<quint16>::max();

        for (int i = 0; i < count; i += channalCount) {
            double v;

            if (is_little) {
                v = qFromLittleEndian(p[i]);
            } else {
                v = qFromBigEndian(p[i]);
            }
            tmpData.append(v / maxVal_);
            dsdata.append(v);
        }
        break;
    }

    case UINT32: {
        count = size / 4;
        const quint32 *p = (quint32 *)sdata.data();
        double maxVal_ = std::numeric_limits<quint32>::max();

        for (int i = 0; i < count; i += channalCount) {
            double v;

            if (is_little) {
                v = qFromLittleEndian(p[i]);
            } else {
                v = qFromBigEndian(p[i]);
            }
            tmpData.append(v / maxVal_);
            dsdata.append(v);
        }
        break;
    }

    case INT32: {
        count = size / 4;
        const qint32 *p = (qint32 *)sdata.data();
        double maxVal_ = std::numeric_limits<qint32>::max();

        for (int i = 0; i < count; i += channalCount) {
            double v;

            if (is_little) {
                v = qFromLittleEndian(p[i]);
            } else {
                v = qFromBigEndian(p[i]);
            }
            tmpData.append(v / maxVal_);
            dsdata.append(v);
        }
        break;
    }

    case FLOAT: {
        count = size / 4;
        const float *p = (float *)sdata.data();

        for (int i = 0; i < count; i += channalCount) {
            double v;

            if (is_little) {
                v = qFromLittleEndian(p[i]);
            } else {
                v = qFromBigEndian(p[i]);
            }
            tmpData.append(v);
            dsdata.append(v);
        }
        break;
    }

    case INT16: {
        count = size / 2;
        const qint16 *p = (qint16 *)sdata.data();
        double maxVal_ = std::numeric_limits<qint16>::max();

        for (int i = 0; i < count; i += channalCount) {
            double v;

            if (is_little) {
                v = qFromLittleEndian(p[i]);
            } else {
                v = qFromBigEndian(p[i]);
            }
            tmpData.append(v / maxVal_);
            dsdata.append(v);
        }
    }

    default:
        break;
    }
    return fft(tmpData, fftdata, radiandata, windowSize, overlap);
}

bool fftw3Object::fftStreamStart(int windowSize, int overlap)
{
    m_windowSize = windowSize;
    m_overlap = overlap;
    m_overlapVec.resize(m_overlap, 0);

    m_window = (double *)fftw_malloc(sizeof(double) * m_windowSize);

    if (!m_window) {
        return false;
    }
    m_input = fftw_alloc_real(m_windowSize);

    if (!m_input) {
        fftw_free(m_window);
        m_window = nullptr;
        return false;
    }
    m_output = fftw_alloc_complex(m_windowSize / 2 + 1);

    if (!m_output) {
        fftw_free(m_window);
        fftw_free(m_input);
        m_input = nullptr;
        m_window = nullptr;
        return false;
    }

    // 只创建一次 plan
    m_plan = fftw_plan_dft_r2c_1d(m_windowSize,
                                  m_input,
                                  m_output,
                                  FFTW_ESTIMATE);

    if (!m_plan) {
        fftw_free(m_window);
        fftw_free(m_input);
        fftw_free(m_output);
        m_input = nullptr;
        m_output = nullptr;
        m_window = nullptr;
        return false;
    }

    // 窗函数通过让数据块头尾平滑衰减到零，
    // 有效减小边界突变，从而压制频谱泄露，让频谱更准确
    // 汉宁窗
    for (int i = 0; i < m_windowSize; ++i) {
        m_window[i] = 0.5 * (1.0 - cos(2.0 * M_PI * i / (m_windowSize - 1)));
    }

    // // 汉明窗
    // for (int i = 0; i < m_windowSize; ++i) {
    //     m_window[i] = 0.54  - 0.46*cos(2.0 * M_PI * i / (m_windowSize - 1));
    // }
    return true;
}

int fftw3Object::fftAddStream(const QVector<double>& in,
                              QVector<double>      & out,
                              QVector<double>      & radiandata)
{
    if (!m_plan) return -1;

    int count = in.size();

    if (count < m_windowSize - m_overlap) return count;

    // 处理重叠(上次的部分数据)，和新数据
    for (int i = 0; i < m_windowSize; i++) {
        if (i < m_overlap) {
            m_input[i] = m_overlapVec[i];
        } else {
            m_input[i] = in[i - m_overlap];
            m_overlapVec[i - m_overlap] = m_input[i];
        }
    }

    // 应用窗函数
    for (int i = 0; i < m_windowSize; ++i) {
        m_input[i] *= m_window[i];
    }

    // 执行FFT
    fftw_execute(m_plan);

    // 计算幅度谱并进行归一化 (真实频谱)
    for (int i = 0; i < m_windowSize / 2 + 1; ++i) {
        double mag = sqrt(
            m_output[i][0] * m_output[i][0] + m_output[i][1] *
            m_output[i][1]);

        // 归一化: 除以窗口长度的一半 (因为r2c的幅度是两倍关系)
        out.append(mag / (m_windowSize / 2));

        // 如果希望幅值以dBFS表示，可以进行对数转换:
        // double db = 20 * log10((mag / (m_windowSize / 2)) + 1e-12);
        // out.append(db);

        // 相位(弧度) atan2:四象限反正切函数，结果范围在[-π, π]之间
        radiandata.append(atan2(m_output[i][1], m_output[i][0]));

        // 频率 f=j*采样率/采样点数
        // double freq = j * 48000 / windowSize;
    }
    return count - (m_windowSize - m_overlap);
}

void fftw3Object::fftStreamSop()
{
    if (!m_plan) return;

    fftw_destroy_plan(m_plan);
    fftw_free(m_input);
    fftw_free(m_output);
    fftw_free(m_window);
    m_plan = nullptr;
    m_input = nullptr;
    m_output = nullptr;
    m_window = nullptr;
}
