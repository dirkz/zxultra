#pragma once

#include <functional>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

namespace zxultra
{

template <class T, class I, class H = std::hash<T>, class E = std::equal_to<T>> struct VertexBuffer
{
    void Add(const T &v)
    {
        auto index = m_vertexIndex.find(v);
        if (index != m_vertexIndex.end())
        {
            auto pair = *index;
            I i = pair.second;
            m_indices.push_back(i);
        }
        else
        {
            I newIndex = static_cast<I>(m_vertices.size());
            m_vertices.push_back(v);
            m_indices.push_back(newIndex);
        }
    }

    template <std::size_t N> void Add(const std::span<const T, N> &vertices)
    {
        for (const T &v : vertices)
        {
            Add(v);
        }
    }

    std::span<T> Vertices()
    {
        return std::span{m_vertices};
    }

    std::span<I> Indices()
    {
        return std::span{m_indices};
    }

  private:
    std::unordered_map<T, I> m_vertexIndex;
    std::vector<T> m_vertices;
    std::vector<I> m_indices;
};

} // namespace zxultra
