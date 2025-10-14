#pragma once

#include "stdafx.h"

namespace zxultra
{

struct VertexWithColor
{
    VertexWithColor(XMFLOAT3 position, FXMVECTOR color) : m_position{position}
    {
        XMStoreFloat4(&m_color, color);
    };

  private:
    XMFLOAT3 m_position;
    XMFLOAT4 m_color;
};

} // namespace zxultra
