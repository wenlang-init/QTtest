#include "WgcCapture.h"
#include <wil/result.h>
#include <iostream>

// 初始化 D3D11 设备
bool WgcCapture::InitializeD3D11Device() {
    // 创建 D3D11 设备和上下文，需要支持 BGRA 格式
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    D3D_FEATURE_LEVEL featureLevels[] =
    { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
    HRESULT hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, creationFlags,
        featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION,
        m_d3dDevice.put(), nullptr, m_d3dContext.put()
        );

    if (FAILED(hr)) {
        std::cerr << "Failed to create D3D11 device. Error: 0x" << std::hex <<
            hr << std::endl;
        return false;
    }
    return true;
}

// 从窗口句柄创建 GraphicsCaptureItem
bool WgcCapture::CreateCaptureItemForWindow(HWND hwnd) {
    // 获取 GraphicsCaptureItem 的工厂
    auto interopFactory = winrt::get_activation_factory<winrt::GraphicsCaptureItem,
                                                        IGraphicsCaptureItemInterop>();

    // 使用 Interop 接口从 HWND 创建 CaptureItem
    winrt::check_hresult(interopFactory->CreateForWindow(hwnd,
                                                         winrt::guid_of<winrt::GraphicsCaptureItem>(),
                                                         winrt::put_abi(
                                                             m_captureItem)));

    if (!m_captureItem) {
        std::cerr << "Failed to create GraphicsCaptureItem from HWND." <<
            std::endl;
        return false;
    }
    return true;
}

// 开始捕获会话
bool WgcCapture::StartCapture() {
    if (!m_d3dDevice || !m_captureItem) {
        std::cerr << "D3D Device or CaptureItem is not initialized." << std::endl;
        return false;
    }

    // 1. 获取 WinRT 的 IDirect3DDevice
    auto d3dDevice = GetD3DDevice();

    if (!d3dDevice) {
        std::cerr << "Failed to get IDirect3DDevice." << std::endl;
        return false;
    }

    // 2. 创建帧池（使用 FreeThreaded 模式，无需 DispatcherQueue）
    m_framePool = winrt::Direct3D11CaptureFramePool::CreateFreeThreaded(
        d3dDevice,
        winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized, // 像素格式
        2,                                                 // 帧池大小
        m_captureItem.Size()                               // 目标尺寸
        );

    if (!m_framePool) {
        std::cerr << "Failed to create CaptureFramePool." << std::endl;
        return false;
    }

    // 3. 创建捕获会话
    m_session = m_framePool.CreateCaptureSession(m_captureItem);

    if (!m_session) {
        std::cerr << "Failed to create CaptureSession." << std::endl;
        return false;
    }

    // 4. 启动捕获（可选：设置是否捕获光标）
    // m_session.IsCursorCaptureEnabled(false);
    m_session.StartCapture();

    m_isCapturing = true;
    std::cout << "Capture started successfully." << std::endl;
    return true;
}

// 获取 WinRT 的 IDirect3DDevice（来自 D3D11 设备）
winrt::IDirect3DDevice WgcCapture::GetD3DDevice() const {
    // 使用 CreateDirect3D11DeviceFromDXGIDevice 从 D3D11 设备创建
    winrt::com_ptr<IDXGIDevice> dxgiDevice;

    winrt::check_hresult(m_d3dDevice->QueryInterface(dxgiDevice.put()));
    winrt::com_ptr<::IInspectable> d3dDevice;
    winrt::check_hresult(CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(),
                                                              d3dDevice.put()));
    return d3dDevice.as<winrt::IDirect3DDevice>();
}

// 捕获单帧并保存为 PNG
bool WgcCapture::CaptureSingleFrameAndSave(const std::wstring& filename) {
    if (!m_framePool) {
        std::cerr << "Frame pool is not initialized. StartCapture first?" <<
            std::endl;
        return false;
    }

    // 1. 从帧池中尝试获取一帧
    auto frame = m_framePool.TryGetNextFrame();

    if (!frame) {
        std::cerr << "Failed to get a frame from the pool." << std::endl;
        return false;
    }

    // 2. 从 WinRT 的 IDirect3DSurface 获取 D3D11 Texture2D
    auto surface = frame.Surface();
    auto interopSurface = surface.as<IDXGISurface>();
    wil::com_ptr<ID3D11Texture2D> texture;
    HRESULT hr = interopSurface->QueryInterface(texture.put());

    if (FAILED(hr)) {
        std::cerr << "Failed to get D3D11 Texture2D from surface. Error: 0x" <<
            std::hex << hr << std::endl;
        return false;
    }

    // 3. 保存纹理为 PNG
    return SaveTextureAsPNG(texture.get(), filename);
}

// 辅助函数：将 D3D11 纹理保存为 PNG（使用 WIC）
bool WgcCapture::SaveTextureAsPNG(ID3D11Texture2D    *texture,
                                  const std::wstring& filename) {
    // 1. 获取纹理描述
    D3D11_TEXTURE2D_DESC desc;

    texture->GetDesc(&desc);

    // 2. 创建与纹理兼容的 Staging Texture，用于 CPU 读取
    desc.Usage = D3D11_USAGE_STAGING;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.BindFlags = 0;
    desc.MiscFlags = 0;

    wil::com_ptr<ID3D11Texture2D> stagingTexture;
    HRESULT hr =
        m_d3dDevice->CreateTexture2D(&desc, nullptr, stagingTexture.put());

    if (FAILED(hr)) {
        std::cerr << "Failed to create staging texture. Error: 0x" << std::hex <<
            hr << std::endl;
        return false;
    }

    // 3. 将渲染纹理复制到 Staging Texture
    m_d3dContext->CopyResource(stagingTexture.get(), texture);

    // 4. 映射 Staging Texture 以获取像素数据
    D3D11_MAPPED_SUBRESOURCE mapped;
    hr = m_d3dContext->Map(stagingTexture.get(), 0, D3D11_MAP_READ, 0, &mapped);

    if (FAILED(hr)) {
        std::cerr << "Failed to map staging texture. Error: 0x" << std::hex <<
            hr << std::endl;
        return false;
    }

    // 5. 使用 Windows Imaging Component (WIC) 将数据编码为 PNG
    bool success = false;

    // 初始化 COM（如果尚未初始化）
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    // ... (WIC 编码代码，与之前类似，此处省略以保持简洁，完整代码见下方说明)
    // 重要：别忘了调用 m_d3dContext->Unmap(stagingTexture.get(), 0);
    // ...

    m_d3dContext->Unmap(stagingTexture.get(), 0);

    // CoUninitialize() 如果成功调用了 CoInitializeEx

    return success;
}

// 停止捕获并清理资源
void WgcCapture::StopCapture() {
    if (m_session) {
        m_session.Close();
        m_session = nullptr;
    }

    if (m_framePool) {
        m_framePool.Close();
        m_framePool = nullptr;
    }
    m_isCapturing = false;
    std::cout << "Capture stopped." << std::endl;
}

// 析构函数
WgcCapture::~WgcCapture() {
    StopCapture();
}

int WgcCaptureTest() {
    // 1. 找到目标窗口（例如：记事本）
    HWND targetHwnd = FindWindowW(L"Notepad", nullptr);

    if (!targetHwnd) {
        std::cerr << "Target window not found." << std::endl;
        return -1;
    }

    // 2. 创建捕获对象并初始化
    WgcCapture capture;

    if (!capture.InitializeD3D11Device()) {
        return -1;
    }

    if (!capture.CreateCaptureItemForWindow(targetHwnd)) {
        return -1;
    }

    // 3. 开始捕获（为获取单帧，这一步是必需的）
    if (!capture.StartCapture()) {
        return -1;
    }

    // 4. 捕获单帧并保存为 PNG
    if (!capture.CaptureSingleFrameAndSave(L"captured_frame.png")) {
        std::cerr << "Failed to capture and save frame." << std::endl;
    } else {
        std::cout << "Frame captured and saved to captured_frame.png" <<
        std::endl;
    }

    // 5. 停止捕获
    capture.StopCapture();

    return 0;
}
