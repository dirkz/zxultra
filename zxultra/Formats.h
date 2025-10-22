#pragma once

#include "stdafx.h"

namespace zxultra
{

constexpr DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

// Note: This must match `IndexFormat'.
using IndexType = std::uint16_t;

// Note: This must match `IndexType'.
constexpr DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;

} // namespace zxultra
