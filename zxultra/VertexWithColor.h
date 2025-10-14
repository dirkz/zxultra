#pragma once

#include "stdafx.h"

#include <Util.h>

namespace zxultra
{

struct VertexWithColor
{
    friend std::hash<zxultra::VertexWithColor>;
    friend bool operator==(const VertexWithColor &v1, const VertexWithColor &v2);

    VertexWithColor(XMFLOAT3 position, FXMVECTOR color) : m_position{position}
    {
        XMStoreFloat4(&m_color, color);
    };

  private:
    XMFLOAT3 m_position;
    XMFLOAT4 m_color;
};

bool operator==(const VertexWithColor &v1, const VertexWithColor &v2)
{
    return v1.m_position.x == v2.m_position.x && v1.m_position.y == v2.m_position.y &&
           v1.m_position.z == v2.m_position.z && v1.m_color.x == v2.m_color.x &&
           v1.m_color.y == v2.m_color.y && v1.m_color.z == v2.m_color.z &&
           v1.m_color.w == v2.m_color.w;
}

} // namespace zxultra

template <> struct std::hash<zxultra::VertexWithColor>
{
    std::size_t operator()(const zxultra::VertexWithColor &v) const noexcept
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
