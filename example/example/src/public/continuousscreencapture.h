#ifndef CONTINUOUSSCREENCAPTURE_H
#define CONTINUOUSSCREENCAPTURE_H

#include <QObject>
#include <QImage>
#include <QAtomicInt>
#include <QThread>
#include <d3d11.h>
#include <dxgi1_2.h>

struct PointerData {
    QImage image;
    int    hotX;
    int    hotY;
    bool   isMaskedColor;
    PointerData() {
        isMaskedColor = false;
    }
};

class ContinuousScreenCapture : public QObject {
    Q_OBJECT

public:

    // explicit ContinuousScreenCapture(HWND     hWnd=nullptr,
    //                                  QObject *parent = nullptr);

    explicit ContinuousScreenCapture(HWND hWnd = nullptr);

    ~ContinuousScreenCapture();

    void start(); // 开始连续捕获
    void stop();  // 停止捕获（阻塞等待线程结束）

    void setScreenSize(int x,
                       int y,
                       int width,
                       int height) {
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;

        m_hWnd = nullptr;
    }

    void setHwnd(HWND hwnd) {
        m_hWnd = hwnd;

        m_x = 0;
        m_y = 0;
        m_width = GetSystemMetrics(SM_CXSCREEN);
        m_height = GetSystemMetrics(SM_CYSCREEN);
    }

    void setLowFpsMode(bool lowfps = false, int fps = 10) {
        m_lowfps = lowfps;
        m_lowfpsinterval = fps > 0 ?
                           (1000 * 1000.0 / (fps + 1)) : (1000 * 1000.0 / 11);
    }

    void setshowMouse(bool show = true) {
        isShowMouse = show;
    }

signals:

    void frameCaptured(const QImage& image); // 每帧捕获完成时发送

private:

    void destroyResources(); // 销毁资源（在析构和重新初始化时调用）
    bool init();
    bool reInit();           // 重新初始化（在捕获循环中调用，以处理设备丢失等情况）

public:

    // 调整视图,以保证在视窗RECT(minX,minY,maxX,maxY)内
    void getViewFinder(int& left, int& top, int& width, int& height,
                       int  minX, int  minY, int  maxX, int  maxY) {
        if (minX > maxX) return;

        if (minY > maxY) return;

        int right = width + left;
        int bottom = height + top;

        if ((right > minX) && (left < maxX)) {
            if (right > maxX) {
                width -= (right - maxX);
            }

            if (left < minX) {
                width -= (minX - left);
                left = minX;
            }
        } else {
            left = 0;
            width = 0;
        }

        if ((bottom > minY) && (top < maxY)) {
            if (bottom > maxY) {
                height -= (bottom - maxY);
            }

            if (top < minY) {
                height -= (minY - top);
                top = minY;
            }
        } else {
            top = 0;
            height = 0;
        }
    }

private:

    void captureLoop(); // 循环体（在单独线程中运行）
    bool getPointerShape(const DXGI_OUTDUPL_FRAME_INFO& frameInfo,
                         IDXGIOutputDuplication        *pDuplication,
                         PointerData                  & result);

    bool m_isinit = false;                     // 是否初始化成功
    QAtomicInt m_running;                      // 控制循环标志
    bool m_lowfps = false;                     // 是否低帧率模式
    int m_lowfpsinterval = 1000 * 1000.0 / 11; // 低帧率模式的间隔时间（微秒）

    bool isShowMouse = true;                   // 是否显示鼠标指针

    // DXGI 资源（在构造时初始化，整个生命周期复用）
    ID3D11Device *m_pDevice;
    ID3D11DeviceContext *m_pContext;
    IDXGIOutputDuplication *m_pDuplication;

    QThread m_workerThread; // 用于运行循环的工作线程

    int m_x, m_y, m_width, m_height;
    HWND m_hWnd;
};

#endif // CONTINUUSSCREENCAPTURE_H
