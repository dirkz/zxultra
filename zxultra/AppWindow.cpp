#include "AppWindow.h"

#include "Error.h"

namespace zxultra
{

void DXWindow::OnHwndCreated(HWND hwnd)
{
#if defined(_DEBUG)
    // Enable the D3D12 debug layer.
    {

        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
        }
    }
#endif

    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_factory)));
    ThrowIfFailed(m_factory->EnumAdapters1(0, &m_adapter));
}

void DXWindow::Resize(int width, int height)
{
}

void DXWindow::Update(double elapsedSeconds)
{
}

void DXWindow::Draw()
{
}

void DXWindow::LogAdapters()
{
    ComPtr<IDXGIFactory1> factory;
    ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

    UINT i = 0;
    ComPtr<IDXGIAdapter1> adapter;
    while (factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC1 desc;
        ThrowIfFailed(adapter->GetDesc1(&desc));

        std::wstring msg = std::format(L"Adapter: {}\n", desc.Description);
        OutputDebugString(msg.c_str());

        LogAdapterOutputs(adapter);

        ++i;
    }
}

void DXWindow::LogAdapterOutputs(ComPtr<IDXGIAdapter1> adapter)
{
    UINT i = 0;
    ComPtr<IDXGIOutput> output;
    while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        ThrowIfFailed(output->GetDesc(&desc));

        std::wstring msg = std::format(L"Output: {}\n", desc.DeviceName);
        OutputDebugString(msg.c_str());

        ++i;
    }
}

} // namespace zxultra
