#ifndef DXGIGETSCREEN_H
#define DXGIGETSCREEN_H

#include <QObject>
#include <d3d11.h>
#include <dxgi1_2.h>

class DXGIGetScreen : public QObject {
    Q_OBJECT

public:

    ~DXGIGetScreen();
    static DXGIGetScreen& getInstance()
    {
        static DXGIGetScreen instance;

        return instance;
    }

    static bool getScreen(QImage& image);
    void        setScreenSize(int x,
                              int y,
                              int width,
                              int height) {
        m_x = x;
        m_y = y;
        m_width = width;
        m_height = height;

        m_hwnd = nullptr;
    }

    void setHwnd(HWND hwnd) {
        m_hwnd = hwnd;
    }

private:

    explicit DXGIGetScreen(QObject *parent = nullptr);
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

signals:

private:

    BOOL Init();
    VOID Deinit();

private:

    // virtual BOOL CaptureImage(void *pData,INT & nLen);
    virtual BOOL CaptureImage(QImage& image);
    virtual BOOL ResetDevice();

private:

    BOOL AttatchToThread(VOID);

    // BOOL QueryFrame(void *pImgData,INT & nImgSize);
    BOOL QueryFrame(QImage& image);

private:

    IDXGIResource *zhDesktopResource;
    DXGI_OUTDUPL_FRAME_INFO zFrameInfo;
    ID3D11Texture2D *zhAcquiredDesktopImage;
    IDXGISurface *zhStagingSurf;

private:

    BOOL m_bInit;
    int m_x, m_y, m_width, m_height;
    HWND m_hwnd;

    ID3D11Device *m_hDevice;
    ID3D11DeviceContext *m_hContext;

    IDXGIOutputDuplication *m_hDeskDupl;
    DXGI_OUTPUT_DESC m_dxgiOutDesc;
};

#endif // DXGIGETSCREEN_H
