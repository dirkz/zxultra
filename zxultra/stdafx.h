#pragma once

#include <format>
#include <stdexcept>
#include <string>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

#include <comdef.h>
#include <windows.h>
#include <wrl.h>

#include <DirectXColors.h>
#include <DirectXMath.h>
#include <Dxgi.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <dxgi1_6.h>

using Microsoft::WRL::ComPtr;
