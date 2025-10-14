#pragma once

#include <functional>

namespace zxultra
{

template <class T, class H = std::hash<T>> inline void hash_combine(std::size_t &seed, const T &v)
{
    H hasher{};
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

} // namespace zxultra
