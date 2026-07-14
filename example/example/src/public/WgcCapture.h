#pragma once
#include <windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <d3d11.h>
#include <wincodec.h>
#include <wil/com.h>
#include <vector>
#include <string>

// 引入必要的命名空间
namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::Graphics::Capture;
using namespace Windows::Graphics::DirectX::Direct3D11;
}

class WgcCapture {
public:

    WgcCapture();
    ~WgcCapture();

    // 初始化 Direct3D 设备
    bool InitializeD3D11Device();

    // 从窗口句柄创建捕获项
    bool CreateCaptureItemForWindow(HWND hwnd);

    // 开始捕获
    bool StartCapture();

    // 停止捕获并清理资源
    void StopCapture();

    // 捕获单帧并保存为 PNG
    bool CaptureSingleFrameAndSave(const std::wstring& filename);

private:

    // 获取 WinRT 的 IDirect3DDevice
    winrt::IDirect3DDevice GetD3DDevice() const;

    // 将 D3D11 纹理保存为 PNG 的辅助函数
    bool                   SaveTextureAsPNG(ID3D11Texture2D    *texture,
                                            const std::wstring& filename);

private:

    // COM 指针
    wil::com_ptr<ID3D11Device>m_d3dDevice;
    wil::com_ptr<ID3D11DeviceContext>m_d3dContext;

    // WinRT 对象
    winrt::GraphicsCaptureItem m_captureItem{ nullptr };
    winrt::Direct3D11CaptureFramePool m_framePool{ nullptr };
    winrt::GraphicsCaptureSession m_session{ nullptr };

    // 状态标志
    bool m_isCapturing = false;
};
