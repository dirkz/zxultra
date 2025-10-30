#pragma once

#include <functional>
#include <initializer_list>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

namespace zxultra
{

template <class T, class I, class H = std::hash<T>, class E = std::equal_to<T>> struct VertexBuffer
{
    VertexBuffer() {};

    VertexBuffer(std::initializer_list<T> vertices)
    {
        for (const T &v : vertices)
        {
            Add(v);
        }
    }

    VertexBuffer(const VertexBuffer &) = delete;

    inline VertexBuffer &operator=(VertexBuffer &&v) noexcept
    {
        std::swap(m_vertexIndex, v.m_vertexIndex);
        std::swap(m_vertices, v.m_vertices);
        std::swap(m_indices, v.m_indices);

        return *this;
    }

    VertexBuffer &operator=(const VertexBuffer &) = delete;

    inline void Add(const T &v)
    {
        auto index = m_vertexIndex.find(v);
        if (index != m_vertexIndex.end())
        {
            I i = index->second;
            m_indices.push_back(i);
        }
        else
        {
            I newIndex = static_cast<I>(m_vertices.size());
            m_vertices.push_back(v);
            m_indices.push_back(newIndex);
            m_vertexIndex.insert(std::make_pair(v, newIndex));
        }
    }

    template <std::size_t N> void Add(const std::span<const T, N> &vertices)
    {
        for (const T &v : vertices)
        {
            Add(v);
        }
    }

    inline void Add(std::initializer_list<T> vertices)
    {
        for (const T &v : vertices)
        {
            Add(v);
        }
    }

    inline std::span<T> Vertices()
    {
        return std::span{m_vertices};
    }

    inline std::span<I> Indices()
    {
        return std::span{m_indices};
    }

    inline unsigned int NumIndices() const
    {
        return static_cast<unsigned int>(m_indices.size());
    }

  private:
    // Note: Move semantics

    std::unordered_map<T, I> m_vertexIndex;
    std::vector<T> m_vertices;
    std::vector<I> m_indices;
};

} // namespace zxultra
