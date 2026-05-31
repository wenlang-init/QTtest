#include "fftw3object.h"
#include "cxxlog.h"

static void test()
{
    int N = 1024;
    int i;
    fftw_complex *din, *out;
    fftw_plan     p;

    din = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * N);

    if ((din == NULL) || (out == NULL))
    {
        FATAL_LOG_CXX("Error:insufficient available memory\n");
    }
    else
    {
        for (i = 0; i < N; i++) /*测试数据*/
        {
            din[i][0] = i + 1;
            din[i][1] = 0;
        }
    }
    p = fftw_plan_dft_1d(N, din, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(p); /* repeat as needed */
    fftw_destroy_plan(p);
    fftw_cleanup();

    INFO_LOG_CXX("OUTPUT:\n");

    for (i = 0; i < N; i++) /*OUTPUT*/
    {
        printf("%f,%fi\n", din[i][0], din[i][1]);
    }
    printf("\n");

    for (i = 0; i < N; i++) /*OUTPUT*/
    {
        printf("%f,%fi\n", out[i][0], out[i][1]);
    }
    fflush(stdout);

    if (din != NULL) fftw_free(din);

    if (out != NULL) fftw_free(out);
}

fftw3Object::fftw3Object(QObject *parent)
    : QObject{parent}
{
    // test();
}

fftw3Object::~fftw3Object()
{
    fftStreamSop();
}

bool fftw3Object::fft(const QByteArray& sdata,
                      QVector<double> & dsdata,
                      QVector<double> & fftdata,
                      int               channalCount,
                      int               byteRate,
                      int               windowSize,
                      int               overlap)
{
    QVector<double> tmpData;
    int size = sdata.size();
    int count = size / byteRate;

    dsdata.clear();

    if (byteRate == 2) {
        const qint16 *p = (qint16 *)sdata.data();

        for (int i = 0; i < count; i += channalCount) {
            double v = p[i];
            v = v / 32768.0; // 归一化
            tmpData.append(v);
            dsdata.append(p[i]);

            // for (int j = i; j < i + channalCount; j++) dsdata.append(p[j]);
        }
    } else if (byteRate == 4) {
        // const qint32 *p = (qint32 *)sdata.data();

        // for (int i = 0; i < count; i+=channalCount) {
        //     double v = p[i];
        //     v = v / (0x7fffffff); // 归一化
        //     tmpData.append(v);
        // }

        // float
        const float *p = (float *)sdata.data();

        for (int i = 0; i < count; i += channalCount) {
            double v = p[i];
            tmpData.append(v);
            dsdata.append(p[i]);

            // for (int j = i; j < i + channalCount; j++) dsdata.append(p[j]);
        }
    } else {
        const qint8 *p = (qint8 *)sdata.data();

        for (int i = 0; i < count; i += channalCount) {
            double v = p[i];
            v = v / 127.0; // 归一化
            tmpData.append(v);
            dsdata.append(p[i]);

            // for (int j = i; j < i + channalCount; j++) dsdata.append(p[j]);
        }
    }


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
            fftdata[j] += mag / (windowSize / 2);
        }
        frame_count++;
    }

    // 计算平均值
    if (frame_count > 1) {
        for (auto& m : fftdata) m /= frame_count;
    }

    // 5. 清理资源
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
    return true;
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

int fftw3Object::fftAddStream(const QVector<double>& in, QVector<double>& out)
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

////////////////////////


#if 0
# include <fftw3.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <math.h>

# define BUFFER_SIZE 1024                     // FFT点数
# define OVERLAP      512                     // 重叠点数 (50% 重叠)
# define STEP_SIZE    (BUFFER_SIZE - OVERLAP) // 每次向前移动的步长

// 预计算窗函数 (如汉宁窗)
double* generate_hanning_window(int n) {
    double *window = (double *)fftw_malloc(sizeof(double) * n);

    for (int i = 0; i < n; ++i) {
        window[i] = 0.5 * (1.0 - cos(2.0 * M_PI * i / (n - 1)));
    }
    return window;
}

// 对单个数据块应用窗函数
void apply_window(double *data, double *window, int n) {
    for (int i = 0; i < n; ++i) {
        data[i] *= window[i];
    }
}

int adafafa() {
    // --- 1. 初始化 FFTW ---
    double *input = fftw_alloc_real(BUFFER_SIZE);
    fftw_complex *output = fftw_alloc_complex(BUFFER_SIZE / 2 + 1);

    // 关键：只创建一次 plan
    fftw_plan plan = fftw_plan_dft_r2c_1d(BUFFER_SIZE,
                                          input,
                                          output,
                                          FFTW_ESTIMATE);

    // --- 2. 初始化窗函数 ---
    double *hanning_window = generate_hanning_window(BUFFER_SIZE);

    // --- 3. 模拟数据流、重叠缓冲区与文件操作 (此处简化)---
    // 实际应用中，您需要从音频设备、传感器等读取float数据流。
    // 这里我们用一段模拟数据来演示流程。
    int total_samples = 5000;
    float *simulated_data = (float *)malloc(total_samples * sizeof(float));

    for (int i = 0; i < total_samples; ++i) {
        // 模拟一个频率为 1000Hz 的正弦波，采样率 44100Hz
        simulated_data[i] = sin(2 * M_PI * 1000.0 * i / 44100.0);
    }

    // 用于存储重叠数据的环形缓冲区，此处使用简单数组模拟
    double ring_buffer[BUFFER_SIZE] = { 0 };
    int    read_pos = 0; // 模拟数据读取位置

    while (read_pos < total_samples) {
        // --- 3.1 从数据流中填充数据到环形缓冲区 ---
        // 移动操作，将 buffer 中 [STEP_SIZE, BUFFER_SIZE) 的数据移到前面
        // 然后从 read_pos 处读取新数据填充到尾部。
        // 这一部分逻辑根据具体应用实现，此处为简化示意。

        // 演示代码：直接拷贝一段数据到 input (缺少重叠逻辑)
        int remaining = total_samples - read_pos;
        int to_copy = STEP_SIZE < remaining ? STEP_SIZE : remaining;

        // 简单移位 (实际应为更高效的memmove)
        for (int i = 0; i < BUFFER_SIZE - STEP_SIZE; i++) {
            ring_buffer[i] = ring_buffer[i + STEP_SIZE];
        }

        for (int i = 0; i < to_copy; i++) {
            ring_buffer[BUFFER_SIZE - STEP_SIZE +
                        i] = simulated_data[read_pos + i];
        }

        if (to_copy < STEP_SIZE) {
            break; // 剩余样本不足一个处理步长，处理结束
        }

        // 将环形缓冲区数据拷贝到 FFT 输入
        memcpy(input, ring_buffer, BUFFER_SIZE * sizeof(double));

        // --- 3.2 应用窗函数 ---
        apply_window(input, hanning_window, BUFFER_SIZE);

        // --- 3.3 执行FFT ---
        fftw_execute(plan); // 实时处理的核心：只调用 execute

        // --- 3.4 处理频域数据 (计算幅度谱等)---
        for (int i = 0; i < BUFFER_SIZE / 2 + 1; ++i) {
            double magnitude = sqrt(
                output[i][0] * output[i][0] + output[i][1] * output[i][1]);
            printf("Block: freq idx %d: mag = %f\n", i, magnitude);
        }

        read_pos += STEP_SIZE;
    }

    // --- 4. 清理资源 ---
    fftw_destroy_plan(plan);
    fftw_free(input);
    fftw_free(output);
    fftw_free(hanning_window);
    free(simulated_data);

    return 0;
}

#endif // if 1
