#include "ContinuousScreenCapture.h"
#include <QDebug>
#include <comdef.h>
#include <QPainter>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

ContinuousScreenCapture::ContinuousScreenCapture(HWND hWnd)
    : QObject(nullptr)
    , m_hWnd(hWnd)
    , m_pDevice(nullptr)
    , m_pContext(nullptr)
    , m_pDuplication(nullptr)
{
    init();

    // 将对象移动到工作线程（但构造函数在主线程，所以稍后移）
    moveToThread(&m_workerThread);
    connect(&m_workerThread,
            &QThread::started,
            this,
            &ContinuousScreenCapture::captureLoop);
}

ContinuousScreenCapture::~ContinuousScreenCapture()
{
    stop(); // 确保停止循环并等待线程结束

    // 释放所有 COM 资源
    if (m_pDuplication) m_pDuplication->Release();

    if (m_pContext) m_pContext->Release();

    if (m_pDevice) m_pDevice->Release();
}

void ContinuousScreenCapture::start()
{
    if (m_running.testAndSetOrdered(0, 1)) {
        m_workerThread.start();
    }
}

void ContinuousScreenCapture::stop()
{
    m_running.testAndSetOrdered(1, 0);

    if (m_workerThread.isRunning()) {
        m_workerThread.quit();
        m_workerThread.wait();
    }
}

bool ContinuousScreenCapture::init()
{
    // 初始化 DXGI 资源（与单次截图相同）
    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &m_pDevice,
        nullptr,
        &m_pContext
        );

    if (FAILED(hr)) {
        qCritical() << "D3D11CreateDevice failed:" << hr;
        return false;
    }

    IDXGIDevice *pDxgiDevice = nullptr;
    hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDxgiDevice);

    if (FAILED(hr)) {
        qCritical() << "QueryInterface IDXGIDevice failed:" << hr;
        return false;
    }

    IDXGIAdapter *pAdapter = nullptr;
    hr = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pAdapter);
    pDxgiDevice->Release();

    if (FAILED(hr)) {
        qCritical() << "GetParent IDXGIAdapter failed:" << hr;
        return false;
    }

    IDXGIOutput *pOutput = nullptr;
    hr = pAdapter->EnumOutputs(0, &pOutput);
    pAdapter->Release();

    if (FAILED(hr)) {
        qCritical() << "EnumOutputs failed:" << hr;
        return false;
    }

    IDXGIOutput1 *pOutput1 = nullptr;
    hr = pOutput->QueryInterface(__uuidof(IDXGIOutput1), (void **)&pOutput1);
    pOutput->Release();

    if (FAILED(hr)) {
        qCritical() << "QueryInterface IDXGIOutput1 failed:" << hr;
        return false;
    }

    hr = pOutput1->DuplicateOutput(m_pDevice, &m_pDuplication);
    pOutput1->Release();

    if (FAILED(hr)) {
        qCritical() << "DuplicateOutput failed:" << hr;
        return false;
    }
    return true;
}

void ContinuousScreenCapture::captureLoop()
{
    // 循环，直到 m_running 被置为 0
    while (m_running.loadAcquire()) {
        // 获取窗口矩形
        RECT windowRect;

        bool isHwnd = true;

        if (!GetWindowRect(m_hWnd, &windowRect)) {
            isHwnd = false;

            // QThread::msleep(10);
            // continue;
        }
        int left = windowRect.left;
        int top = windowRect.top;
        int width = windowRect.right - windowRect.left;
        int height = windowRect.bottom - windowRect.top;

        if (isHwnd && ((width <= 0) || (height <= 0))) {
            QThread::msleep(10);
            continue;
        }

        // 捕获一帧
        IDXGIResource *pDesktopResource = nullptr;
        DXGI_OUTDUPL_FRAME_INFO frameInfo;
        HRESULT hr = m_pDuplication->AcquireNextFrame(500,
                                                      &frameInfo,
                                                      &pDesktopResource);

        if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
            // 超时，继续下一轮循环（有新帧时再捕获）
            continue;
        }

        if (FAILED(hr)) {
            qWarning() << "AcquireNextFrame failed:" << hr;
            QThread::msleep(10);
            continue;
        }

        // 获取桌面纹理
        ID3D11Texture2D *pDesktopTexture = nullptr;
        hr = pDesktopResource->QueryInterface(__uuidof(ID3D11Texture2D),
                                              (void **)&pDesktopTexture);
        pDesktopResource->Release();

        if (FAILED(hr)) {
            qWarning() << "QueryInterface for texture failed:" << hr;
            m_pDuplication->ReleaseFrame();
            continue;
        }

        // 创建 Staging 纹理（复用？可优化为缓存，但为清晰这里每帧新建）
        D3D11_TEXTURE2D_DESC desc;
        pDesktopTexture->GetDesc(&desc);
        D3D11_TEXTURE2D_DESC stagingDesc = desc;
        stagingDesc.Usage = D3D11_USAGE_STAGING;
        stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        stagingDesc.BindFlags = 0;
        stagingDesc.MiscFlags = 0;

        ID3D11Texture2D *pStagingTexture = nullptr;
        hr = m_pDevice->CreateTexture2D(&stagingDesc, nullptr, &pStagingTexture);

        if (FAILED(hr)) {
            qWarning() << "Create staging texture failed:" << hr;
            pDesktopTexture->Release();
            m_pDuplication->ReleaseFrame();
            continue;
        }

        m_pContext->CopyResource(pStagingTexture, pDesktopTexture);
        pDesktopTexture->Release();

        // 映射并裁剪窗口区域
        D3D11_MAPPED_SUBRESOURCE mapped;
        hr = m_pContext->Map(pStagingTexture, 0, D3D11_MAP_READ, 0, &mapped);

        if (SUCCEEDED(hr)) {
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);

            if (screenWidth > mapped.RowPitch / 4) {
                screenWidth = mapped.RowPitch / 4;
            }

            if (screenHeight > mapped.DepthPitch / 4) {
                screenHeight = mapped.DepthPitch / 4;
            }

            if (!isHwnd) {
                left = 0;
                top = 0;
                width = screenWidth;
                height = screenHeight;
            } else {
                // 调整视图,以保证在视窗(0,0,screenWidth,screenHeight)内
                getViewFinder(left, top, width, height,
                              0, 0, screenWidth, screenHeight);
            }
#if 0
            QImage result = QImage((const uchar *)mapped.pData,
                                   screenWidth,
                                   screenHeight,
                                   mapped.RowPitch,
                                   QImage::Format_ARGB32
                                   ).copy(left, top, width, height);
#else // if 1
            const int bytesPerPixel = 4;
            QImage    result(width, height, QImage::Format_ARGB32);

            for (int y = 0; y < height; ++y) {
                const quint8 *srcRow = static_cast<const quint8 *>(mapped.pData) +
                                       (top + y) * mapped.RowPitch +
                                       left * bytesPerPixel;
                quint8 *dstRow = result.scanLine(y);

                memcpy(dstRow, srcRow, width * bytesPerPixel);

                // // BGRA -> ARGB 转换
                // for (int x = 0; x < width; ++x) {
                //     const quint8 *src = srcRow + x * bytesPerPixel;
                //     quint8 *dst = dstRow + x * bytesPerPixel;
                //     dst[0] = src[2]; // R
                //     dst[1] = src[1]; // G
                //     dst[2] = src[0]; // B
                //     dst[3] = src[3]; // A
                // }
            }
#endif // if 1

            // 处理鼠标光标
            // if (frameInfo.PointerShapeBufferSize > 0) {
            //     PointerData data = getPointerShape(frameInfo);

            //     if (!data.image.isNull()) {
            //         int posX = frameInfo.PointerPosition.Position.x - left -
            //                    data.hotX;
            //         int posY = frameInfo.PointerPosition.Position.y - top -
            //                    data.hotY;
            //         QPainter painter(&result);
            //         // painter.setPen(Qt::red);             // 文字颜色
            //         // painter.setFont(QFont("Arial", 50)); // 字体
            //         // painter.drawText(50, 50, "saaaaaaaaaaa");
            //         painter.drawImage(posX, posY, data.image);
            //         painter.end();
            //     }
            // }

            // 发送信号（跨线程安全）
            if (!result.isNull()) emit frameCaptured(result);

            m_pContext->Unmap(pStagingTexture, 0);
        } else {
            qWarning() << "Map failed:" << hr;
        }

        pStagingTexture->Release();

        // 释放当前帧资源（必须）
        m_pDuplication->ReleaseFrame();
    }
}

PointerData ContinuousScreenCapture::getPointerShape(
    const DXGI_OUTDUPL_FRAME_INFO& frameInfo)
{
    PointerData result;

    if (frameInfo.PointerShapeBufferSize == 0) {
        return result; // 空图像
    }

    std::vector<BYTE> shapeBuffer(frameInfo.PointerShapeBufferSize);
    DXGI_OUTDUPL_POINTER_SHAPE_INFO shapeInfo;
    UINT bufferSize = frameInfo.PointerShapeBufferSize;

    HRESULT hr = m_pDuplication->GetFramePointerShape(
        bufferSize, shapeBuffer.data(), &bufferSize, &shapeInfo);

    if (FAILED(hr) || (shapeInfo.Width == 0) || (shapeInfo.Height == 0)) {
        return result;
    }

    // 保存热区
    result.hotX = shapeInfo.HotSpot.x;
    result.hotY = shapeInfo.HotSpot.x;

    QImage cursorImg(shapeInfo.Width, shapeInfo.Height, QImage::Format_ARGB32);
    cursorImg.fill(Qt::transparent);

    const BYTE *bits = shapeBuffer.data();

    switch (shapeInfo.Type) {
    case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR: {
        const quint32 *src = reinterpret_cast<const quint32 *>(bits);

        for (int y = 0; y < shapeInfo.Height; ++y) {
            QRgb *dst = reinterpret_cast<QRgb *>(cursorImg.scanLine(y));

            for (int x = 0; x < shapeInfo.Width; ++x) {
                quint32 pixel = src[y * shapeInfo.Width + x];
                dst[x] = qRgba(
                    (pixel >> 16) & 0xFF,
                    (pixel >> 8) & 0xFF,
                    pixel & 0xFF,
                    (pixel >> 24) & 0xFF
                    );
            }
        }
        break;
    }

    case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME: {
        int pitch = (shapeInfo.Width + 7) / 8;
        const BYTE *andMask = bits;
        const BYTE *xorMask = bits + pitch * shapeInfo.Height;

        for (int y = 0; y < shapeInfo.Height; ++y) {
            QRgb *dst = reinterpret_cast<QRgb *>(cursorImg.scanLine(y));

            for (int x = 0; x < shapeInfo.Width; ++x) {
                int  byteIdx = x / 8;
                int  bitIdx = 7 - (x % 8);
                bool andBit = (andMask[y * pitch + byteIdx] >> bitIdx) & 1;
                bool xorBit = (xorMask[y * pitch + byteIdx] >> bitIdx) & 1;

                if (andBit) {
                    dst[x] = qRgba(0, 0, 0, 0);         // 透明
                } else if (xorBit) {
                    dst[x] = qRgba(255, 255, 255, 255); // 白色
                } else {
                    dst[x] = qRgba(0, 0, 0, 255);       // 黑色
                }
            }
        }
        break;
    }

    case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MASKED_COLOR: {
        const quint32 *src = reinterpret_cast<const quint32 *>(bits);
        const BYTE    *mask = bits + shapeInfo.Width * shapeInfo.Height * 4;

        for (int y = 0; y < shapeInfo.Height; ++y) {
            QRgb *dst = reinterpret_cast<QRgb *>(cursorImg.scanLine(y));
            int   maskOffset = y * ((shapeInfo.Width + 7) / 8);

            for (int x = 0; x < shapeInfo.Width; ++x) {
                quint32 pixel = src[y * shapeInfo.Width + x];
                int     byteIdx = x / 8;
                int     bitIdx = 7 - (x % 8);
                bool    maskBit = (mask[maskOffset + byteIdx] >> bitIdx) & 1;

                if (maskBit) {
                    dst[x] = qRgba(
                        (pixel >> 16) & 0xFF,
                        (pixel >> 8) & 0xFF,
                        pixel & 0xFF,
                        (pixel >> 24) & 0xFF
                        );
                } else {
                    dst[x] = qRgba(0, 0, 0, 0);
                }
            }
        }
        break;
    }

    default:
        return result;
    }

    result.image = cursorImg;
    return result;
}
