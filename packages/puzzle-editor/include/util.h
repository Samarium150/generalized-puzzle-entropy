#pragma once

#include <iterator>

/**
 * polyfill for std::ranges::contains in C++17
 * @tparam C container type
 * @tparam T value type
 * @param c container
 * @param v value
 * @see https://codereview.stackexchange.com/a/59999
 */
template <class C, class T>
constexpr auto contains(const C &c, const T &v) -> decltype(std::end(c), true) {
    return std::end(c) != std::find(std::begin(c), std::end(c), v);
}
