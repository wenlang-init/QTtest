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

private:

    explicit DXGIGetScreen(QObject *parent = nullptr);

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
    int m_iWidth, m_iHeight;

    ID3D11Device *m_hDevice;
    ID3D11DeviceContext *m_hContext;

    IDXGIOutputDuplication *m_hDeskDupl;
    DXGI_OUTPUT_DESC m_dxgiOutDesc;
};

#endif // DXGIGETSCREEN_H
