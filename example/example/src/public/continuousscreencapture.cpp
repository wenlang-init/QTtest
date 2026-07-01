#include "ContinuousScreenCapture.h"
#include <QDebug>
#include <QDateTime>
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
    m_x = 0;
    m_y = 0;
    m_width = GetSystemMetrics(SM_CXSCREEN);
    m_height = GetSystemMetrics(SM_CYSCREEN);

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
    destroyResources();
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

void ContinuousScreenCapture::destroyResources()
{
    if (m_pDuplication) {
        m_pDuplication->Release();
        m_pDuplication = nullptr;
    }

    if (m_pContext) {
        m_pContext->Release();
        m_pContext = nullptr;
    }

    if (m_pDevice) {
        m_pDevice->Release();
        m_pDevice = nullptr;
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
        destroyResources();
        return false;
    }

    IDXGIDevice *pDxgiDevice = nullptr;
    hr = m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDxgiDevice);

    if (FAILED(hr)) {
        qCritical() << "QueryInterface IDXGIDevice failed:" << hr;
        destroyResources();
        return false;
    }

    IDXGIAdapter *pAdapter = nullptr;
    hr = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pAdapter);
    pDxgiDevice->Release();

    if (FAILED(hr)) {
        qCritical() << "GetParent IDXGIAdapter failed:" << hr;
        destroyResources();
        return false;
    }

    IDXGIOutput *pOutput = nullptr;
    hr = pAdapter->EnumOutputs(0, &pOutput);
    pAdapter->Release();

    if (FAILED(hr)) {
        qCritical() << "EnumOutputs failed:" << hr;
        destroyResources();
        return false;
    }

    IDXGIOutput1 *pOutput1 = nullptr;
    hr = pOutput->QueryInterface(__uuidof(IDXGIOutput1), (void **)&pOutput1);
    pOutput->Release();

    if (FAILED(hr)) {
        qCritical() << "QueryInterface IDXGIOutput1 failed:" << hr;
        destroyResources();
        return false;
    }

    hr = pOutput1->DuplicateOutput(m_pDevice, &m_pDuplication);
    pOutput1->Release();

    if (FAILED(hr)) {
        qCritical() << "DuplicateOutput failed:" << hr;
        destroyResources();
        return false;
    }
    m_isinit = true;
    return true;
}

bool ContinuousScreenCapture::reInit()
{
    destroyResources();
    m_isinit = false;
    return init();
}

void ContinuousScreenCapture::captureLoop()
{
    // 由于每帧耗时差距太大，并不能控制帧率
    // 这里实现的是 控制帧的间隔不低于m_lowfpsinterval
    QDateTime dateTime = QDateTime::currentDateTime();
    qint64    timeout;
    qint64    sleepTime;
    quint64   lastcount = 0;
    quint64   count = 0;

    D3D11_TEXTURE2D_DESC stagingDesc;

    PointerData m_pointerData; // 鼠标指针数据（在捕获循环中更新）
    int mouseX = 0;            // 记录鼠标位置
    int mouseY = 0;

    // 循环，直到 m_running 被置为 0
    while (m_running.loadAcquire()) {
        if (!m_isinit) {
            init();
            QThread::msleep(100);
            continue;
        }

        if (m_lowfps) {
            if (lastcount != count) {
                timeout = dateTime.msecsTo(QDateTime::currentDateTime());

                sleepTime = m_lowfpsinterval - (timeout * 1000);

                if (sleepTime > 0) {
                    QThread::usleep(sleepTime);
                }
                dateTime = QDateTime::currentDateTime();
                lastcount = count;
            }
        }

        int left = m_x;
        int top = m_y;
        int width = m_width;
        int height = m_height;

        if (m_hWnd) {
            RECT windowRect;

            if (GetWindowRect(m_hWnd, &windowRect)) {
                left = windowRect.left;
                top = windowRect.top;
                width = windowRect.right - windowRect.left;
                height = windowRect.bottom - windowRect.top;
            }
        }

        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);

        // 调整视图,以保证在视窗(0,0,screenWidth,screenHeight)内
        getViewFinder(left, top, width, height,
                      0, 0, screenWidth, screenHeight);

        if ((width < 1) || (height < 1)) {
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

            // if (DXGI_ERROR_ACCESS_LOST == hr)
            {
                reInit();
                QThread::msleep(100);
            }
            continue;
        }

        {
            // 鼠标数据获取 ,立刻读取并缓存鼠标数据（CPU 轻量操作）
            if (frameInfo.PointerPosition.Visible) {
                mouseX = frameInfo.PointerPosition.Position.x;
                mouseY = frameInfo.PointerPosition.Position.y;
            }

            PointerData data;

            if (getPointerShape(frameInfo, m_pDuplication, data)) {
                m_pointerData = data;
            }
        }

        // 获取桌面纹理 可能触发 GPU 阻塞）
        ID3D11Texture2D *pDesktopTexture = nullptr;
        hr = pDesktopResource->QueryInterface(__uuidof(ID3D11Texture2D),
                                              (void **)&pDesktopTexture);
        pDesktopResource->Release();

        if (FAILED(hr)) {
            qWarning() << "QueryInterface for texture failed:" << hr;
            m_pDuplication->ReleaseFrame();
            continue;
        }

        // 创建 Staging 纹理
        pDesktopTexture->GetDesc(&stagingDesc);
        stagingDesc.Usage = D3D11_USAGE_STAGING;
        stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        stagingDesc.BindFlags = 0;
        stagingDesc.MiscFlags = 0;

        // stagingDesc.MipLevels = 1;
        // stagingDesc.ArraySize = 1;
        // stagingDesc.SampleDesc.Count = 1;

        // stagingDesc.Format = DXGI_FORMAT_R8G8B8A8_UINT;

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
#if 0
            QImage result = QImage((const uchar *)mapped.pData,
                                   screenWidth,
                                   screenHeight,
                                   mapped.RowPitch,
                                   QImage::Format_ARGB32
                                   ).copy(left, top, width, height);
#else // if 0
            const int bytesPerPixel = 4;
            QImage    result(width, height, QImage::Format_ARGB32);

            for (int y = 0; y < height; ++y) {
                const quint8 *srcRow = static_cast<const quint8 *>(mapped.pData) +
                                       (top + y) * mapped.RowPitch +
                                       left * bytesPerPixel;
                quint8 *dstRow = result.scanLine(y);

                memcpy(dstRow, srcRow, width * bytesPerPixel);

                // ARGB -> ARGB 转换
                // for (int x = 0; x < width; ++x) {
                //     const quint8 *src = srcRow + x * bytesPerPixel;
                //     quint8 *dst = dstRow + x * bytesPerPixel;
                //     dst[0] = src[0];
                //     dst[1] = src[1];
                //     dst[2] = src[2];
                //     dst[3] = src[3];
                // }
            }
#endif // if 1

            if (!result.isNull()) {
                // 处理鼠标光标
                if (isShowMouse && !m_pointerData.image.isNull()) {
                    int posX = mouseX - left + m_pointerData.hotX;
                    int posY = mouseY - top + m_pointerData.hotY;
                    QPainter painter(&result);

                    if (m_pointerData.isMaskedColor)
                    {
                        int yStart = posY;
                        int yHeight = m_pointerData.image.height();
                        int xStart = posX;
                        int xWidth = m_pointerData.image.width();
                        const int bytesPerPixel = 4;

                        if (yStart < 0) yStart = 0;

                        if (xStart < 0) xStart = 0;

                        for (int y = 0; y < yHeight; ++y) {
                            if (y + yStart >= result.height()) break;
                            const quint8 *srcRow =
                                m_pointerData.image.scanLine(y);
                            quint8 *dstRow = result.scanLine(y + yStart) +
                                             xStart * bytesPerPixel;

                            for (int x = 0; x < xWidth; x++) {
                                if (x + xStart >= result.width()) break;
                                const quint8 *src = srcRow + x * bytesPerPixel;
                                quint8 *dst = dstRow + x * bytesPerPixel;

                                if (src[3] == 0) {
                                    dst[0] = src[0];
                                    dst[1] = src[1];
                                    dst[2] = src[2];
                                } else {
                                    // 0xff
                                    dst[0] ^= src[0];
                                    dst[1] ^= src[1];
                                    dst[2] ^= src[2];
                                }
                            }
                        }

                        // painter.drawImage(posX, posY, m_pointerData.image);
                    } else {
                        painter.drawImage(posX, posY, m_pointerData.image);
                    }
                    painter.end();
                }
                emit frameCaptured(result);
            }
            m_pContext->Unmap(pStagingTexture, 0);
        } else {
            qWarning() << "Map failed:" << hr;
        }
        pStagingTexture->Release();

        // 释放当前帧资源（必须）
        m_pDuplication->ReleaseFrame();

        count++;
    }
}

bool ContinuousScreenCapture::getPointerShape(
    const DXGI_OUTDUPL_FRAME_INFO& frameInfo,
    IDXGIOutputDuplication        *pDuplication,
    PointerData                  & result)
{
    if (frameInfo.PointerShapeBufferSize <= 0) {
        return false; // 空图像
    }

    std::vector<BYTE> shapeBuffer(frameInfo.PointerShapeBufferSize);
    DXGI_OUTDUPL_POINTER_SHAPE_INFO shapeInfo;
    UINT bufferSize = frameInfo.PointerShapeBufferSize;

    HRESULT hr = pDuplication->GetFramePointerShape(
        bufferSize, shapeBuffer.data(), &bufferSize, &shapeInfo);

    if (FAILED(hr) || (shapeInfo.Width == 0) || (shapeInfo.Height == 0)) {
        return false;
    }

    // 保存热区
    result.hotX = shapeInfo.HotSpot.x;
    result.hotY = shapeInfo.HotSpot.x;

    QImage cursorImg(shapeInfo.Width, shapeInfo.Height, QImage::Format_ARGB32);
    cursorImg.fill(Qt::transparent);

    const BYTE *bits = shapeBuffer.data();

    switch (shapeInfo.Type) {
    case DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR: {
        // cursorImg = QImage(bits,
        //                    shapeInfo.Width,
        //                    shapeInfo.Height,
        //                    QImage::Format_ARGB32); break;
        const quint32 *src = reinterpret_cast<const quint32 *>(bits);

        for (int y = 0; y < shapeInfo.Height; ++y) {
            const quint8 *srcRow =
                reinterpret_cast<const quint8 *>(src + y * shapeInfo.Width);
            quint8 *dstRow = cursorImg.scanLine(y);
            memcpy(dstRow, srcRow, shapeInfo.Width * 4);

            // QRgb *dst = reinterpret_cast<QRgb *>(cursorImg.scanLine(y));
            // for (int x = 0; x < shapeInfo.Width; ++x) {
            //     // quint8 *pixel = (quint8 *)(&src[y * shapeInfo.Width + x]);
            //     // argb->argb;
            //     // dst[x] = qRgba(pixel[2], pixel[1], pixel[0], pixel[3]);
            //     quint32 pixel = src[y * shapeInfo.Width + x];
            //     dst[x] = qRgba(
            //         (pixel >> 16) & 0xFF,
            //         (pixel >> 8) & 0xFF,
            //         pixel & 0xFF,
            //         (pixel >> 24) & 0xFF
            //         );
            // }
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
        {
            result.isMaskedColor = true;
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
        const quint32 *src = reinterpret_cast<const quint32 *>(bits);
        const BYTE *mask = bits + shapeInfo.Width * shapeInfo.Height * 4;

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
        return false;
    }

    result.image = cursorImg;
    return true;
}
