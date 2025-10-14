#pragma once

#include "stdafx.h"

#include <Util.h>

namespace zxultra
{

struct VertexWithColor
{
    friend std::hash<zxultra::VertexWithColor>;

    VertexWithColor(XMFLOAT3 position, FXMVECTOR color) : m_position{position}
    {
        XMStoreFloat4(&m_color, color);
    };

  private:
    XMFLOAT3 m_position;
    XMFLOAT4 m_color;
};

} // namespace zxultra

template <> struct std::hash<zxultra::VertexWithColor>
{
    std::size_t operator()(const zxultra::VertexWithColor &v)
    {
        using zxultra::hash_combine;

        std::hash<float> floatHasher{};

        std::size_t h = floatHasher(v.m_position.x);
        hash_combine(h, floatHasher(v.m_position.y));
        hash_combine(h, floatHasher(v.m_position.z));

        hash_combine(h, floatHasher(v.m_color.x));
        hash_combine(h, floatHasher(v.m_color.y));
        hash_combine(h, floatHasher(v.m_color.z));
        hash_combine(h, floatHasher(v.m_color.w));

        return h;
    }
};
