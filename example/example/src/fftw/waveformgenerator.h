#ifndef WAVEFORMGENERATOR_H
#define WAVEFORMGENERATOR_H

#include <vector>
#include <cmath>
#include <random>
#include <limits>
#include <algorithm>
#include <stdexcept>

// #include <cstdint>

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif // ifndef M_PI

// 支持的波形类型
enum class WaveformType {
    Sine,     // 正弦波
    Square,   // 方波（占空比50%）
    Triangle, // 三角波
    Sawtooth, // 锯齿波（上升锯齿）
    Noise     // 白噪声（均匀分布）
};

/**
 * @brief 多功能波形生成器（连续生成，相位连续）
 * @tparam SampleType 样本数据类型（float, double, int16_t, int32_t...）
 */
template<typename SampleType>
class WaveformGenerator {
public:

    /**
     * @param sampleRate   采样率（Hz）>0
     * @param frequency    信号频率（Hz）>=0（噪声时忽略）
     * @param amplitude    幅度（浮点类型为峰值；整型为归一化峰值 [0,1]）
     * @param firstPhase   初相 [0~2π]
     * @param channels     通道数 >0，所有通道输出相同波形
     * @param type         波形类型，默认为正弦波
     * @throws std::invalid_argument 参数非法时抛出
     */
    WaveformGenerator(double       sampleRate,
                      double       frequency,
                      double       amplitude,
                      double       firstPhase,
                      int          channels,
                      WaveformType type = WaveformType::Sine)
        : sampleRate_(sampleRate), frequency_(frequency), amplitude_(amplitude),
        channels_(channels), type_(type), phase_(firstPhase), invSampleRate_(
            1.0 / sampleRate) {
        if ((sampleRate_ <= 0.0) || (frequency_ < 0.0) || (amplitude_ < 0.0) ||
            (channels_ <= 0)) {
            throw std::invalid_argument("Invalid parameters for WaveformGenerator");
        }

        constexpr bool isInteger = std::is_integral_v<SampleType>;
        if constexpr (isInteger) {
            if (amplitude_ > 1.0) {
                throw std::invalid_argument(
                          "Amplitude for integer types must be in [0,1]");
            }
            maxVal_ = std::numeric_limits<SampleType>::max();
            minVal_ = std::numeric_limits<SampleType>::min();
        }

        // 预计算相位增量
        updateDeltaPhase();

        // 噪声生成器初始化
        if (type_ == WaveformType::Noise) {
            noiseGen_ = std::mt19937(rd_());
            noiseDist_ = std::uniform_real_distribution<double>(-1.0, 1.0);
        }
    }

    /**
     * @brief 改变波形类型（相位重置可选）
     * @param type      新波形类型
     * @param resetPhase 是否重置相位（默认false，保持连续性）
     */
    void setWaveformType(WaveformType type, bool resetPhase = false) {
        type_ = type;

        if (resetPhase) phase_ = 0.0;
    }

    /**
     * @brief 改变频率（保持相位连续性）
     */
    void setFrequency(double newFrequency) {
        frequency_ = newFrequency;
        updateDeltaPhase();
    }

    /**
     * @brief 改变幅度
     */
    void setAmplitude(double newAmplitude) {
        amplitude_ = newAmplitude;
    }

    /**
     * @brief 重置相位到指定弧度
     */
    void resetPhase(double initialPhase = 0.0) {
        phase_ = initialPhase;
    }

    /**
     * @brief 生成单个样本值（单通道）
     */
    SampleType nextSample() {
        double value = 0.0;

        if (type_ == WaveformType::Noise) {
            value = noiseDist_(noiseGen_);
        } else {
            // 计算归一化波形值 [-1, 1]
            double normPhase = phase_ / (2.0 * M_PI); // [0, 1)
            value = computeWaveformValue(normPhase);

            // 更新相位
            phase_ += deltaPhase_;

            if (phase_ >= 2.0 * M_PI) {
                phase_ -= 2.0 * M_PI;
            }
        }

        // 缩放幅度并转换数据类型
        double scaled = amplitude_ * value;
        return convertSample(scaled);
    }

    /**
     * @brief 生成一个多通道帧（所有通道值相同）
     * @return std::vector<SampleType> 长度为 channels_
     */
    std::vector<SampleType>nextFrame() {
        SampleType single = nextSample();

        return std::vector<SampleType>(channels_, single);
    }

    /**
     * @brief 生成一批交错格式的连续样本
     * @param numSamples 要生成的样本总数（每个通道计数，即交错数组长度）
     * @return std::vector<SampleType> 长度为 numSamples 的交错数据
     */
    std::vector<SampleType>generateBlock(size_t numSamples) {
        std::vector<SampleType> block;

        block.reserve(numSamples);

        for (size_t i = 0; i < numSamples; ++i) {
            SampleType s = nextSample();

            for (int ch = 0; ch < channels_; ++ch) {
                block.push_back(s);
            }
        }
        return block;
    }

    // 查询当前状态
    double getPhase() const {
        return phase_;
    }

    int getChannels() const {
        return channels_;
    }

    double getSampleRate() const {
        return sampleRate_;
    }

    double getFrequency() const {
        return frequency_;
    }

    WaveformType getWaveformType() const {
        return type_;
    }

private:

    /**
     * @brief 根据归一化相位 [0,1) 计算波形幅度 [-1,1]
     */
    double computeWaveformValue(double phaseNorm) {
        switch (type_) {
        case WaveformType::Sine:
            return std::sin(2.0 * M_PI * phaseNorm);

        case WaveformType::Square:

            // 方波：相位 < 0.5 时为 +1，否则 -1
            return (phaseNorm < 0.5) ? 1.0 : -1.0;

        case WaveformType::Triangle:

            // 三角波：在 [0,0.5) 上升，[0.5,1) 下降
            if (phaseNorm < 0.25) {
                return 4.0 * phaseNorm;        // 0 -> 1
            } else if (phaseNorm < 0.75) {
                return 2.0 - 4.0 * phaseNorm;  // 1 -> -1
            } else {
                return -4.0 + 4.0 * phaseNorm; // -1 -> 0
            }

        case WaveformType::Sawtooth:

            // 锯齿波：从 -1 线性增长到 +1
            return 2.0 * phaseNorm - 1.0;

        case WaveformType::Noise:

            // 噪声不在此分支处理，因为不需要相位更新
            return 0.0; // never reached

        default:
            return 0.0;
        }
    }

    void updateDeltaPhase() {
        deltaPhase_ = 2.0 * M_PI * frequency_ * invSampleRate_;
    }

    SampleType convertSample(double scaledValue) {
        constexpr bool isInteger = std::is_integral_v<SampleType>;
        if constexpr (isInteger) {
            // 整型：将范围 [-amplitude, amplitude] 映射到整型范围
            // 注意 amplitude>0，且 scaledValue 范围已在 [-amplitude, amplitude]
            double intVal = scaledValue  * maxVal_;
            intVal =
                std::clamp(intVal, static_cast<double>(minVal_),
                           static_cast<double>(maxVal_));
            return static_cast<SampleType>(std::llround(intVal));
        } else {
            // 浮点直接返回
            return static_cast<SampleType>(scaledValue);
        }
    }

    double sampleRate_;
    double frequency_;
    double amplitude_;
    int channels_;
    WaveformType type_;
    double phase_;      // 当前相位（弧度）
    double deltaPhase_; // 每采样点相位增量
    double invSampleRate_;

    // 整型转换辅助
    double maxVal_;
    double minVal_;

    // 噪声生成器（仅当类型为 Noise 时使用）
    std::random_device rd_;
    std::mt19937 noiseGen_;
    std::uniform_real_distribution<double>noiseDist_;
};

#if 0
# include <iostream>

// #include <iomanip>

int testWaveformGenerator() {
    try {
        // 1. 生成双通道正弦波（float）
        WaveformGenerator<float> sineGen(48000.0,
                                         440.0,
                                         0.8f,
                                         2,
                                         WaveformType::Sine);
        std::cout << "Sine wave, first 5 frames:\n";

        for (int i = 0; i < 5; ++i) {
            auto frame = sineGen.nextFrame();
            std::cout << "Frame " << i << ": L=" << frame[0] << ", R=" <<
                frame[1] << std::endl;
        }

        // 2. 生成单声道方波（int16_t）
        WaveformGenerator<int16_t> squareGen(16000.0,
                                             1000.0,
                                             0.5,
                                             1,
                                             WaveformType::Square);
        std::cout << "\nSquare wave (int16_t), first 8 samples:\n";

        for (int i = 0; i < 8; ++i) {
            std::cout << squareGen.nextSample() << " ";
        }
        std::cout << std::endl;

        // 3. 生成三角波并动态改变频率
        WaveformGenerator<double> triGen(44100.0,
                                         200.0,
                                         0.6,
                                         1,
                                         WaveformType::Triangle);
        std::cout << "\nTriangle wave, frequency sweep (phase continuous):\n";

        for (int i = 0; i < 100; ++i) {
            if (i == 50) {
                triGen.setFrequency(400.0);
                std::cout << "\nFrequency changed to 400 Hz at sample 50\n";
            }

            if (i % 25 == 0) {
                std::cout << triGen.nextSample() << " ";
            } else {
                triGen.nextSample();
            }
        }
        std::cout << std::endl;

        // 4. 生成白噪声（双声道，浮点）
        WaveformGenerator<float> noiseGen(48000.0,
                                          0.0,
                                          0.3,
                                          2,
                                          WaveformType::Noise);
        std::cout << "\nWhite noise, first 8 samples (interleaved L,R):\n";
        auto noiseBlock = noiseGen.generateBlock(16); // 16个样本 = 8帧

        for (size_t i = 0; i < noiseBlock.size(); ++i) {
            std::cout << noiseBlock[i] << (i % 2 == 1 ? "\n" : " ");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}

#endif // if 1

#endif // ifndef WAVEFORMGENERATOR_H
