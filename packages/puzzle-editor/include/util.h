#pragma once

#include <fstream>
#include <iterator>
#include <sstream>

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

/**
 * Count the number of lines in a file.
 * @param is input stream
 * @return number of lines
 * @see https://stackoverflow.com/a/37696450
 */
inline auto count(std::istream &is) {
    if (!is || is.bad()) return static_cast<std::size_t>(0);
    const auto state = is.rdstate();
    is.clear();
    const auto pos = is.tellg();

    is.seekg(0);
    std::size_t count =
        std::count(std::istreambuf_iterator(is), std::istreambuf_iterator<char>(), '\n');

    if (is.tellg() != 0) {
        is.unget();
        if (is.get() != '\n') ++count;
    }

    is.clear();
    is.seekg(pos);
    is.setstate(state);

    return count;
}

inline std::string GetLastLine(std::istream &is) {
    const auto state = is.rdstate();
    is.clear();
    const auto pos = is.tellg();
    is.seekg(-1, std::ios_base::end);
    while (true) {
        char ch;
        is.get(ch);
        if (is.tellg() <= 1) {
            is.seekg(0);
            break;
        }
        if (ch == '\n') break;
        is.seekg(-2, std::ios_base::cur);
    }
    std::string line;
    std::getline(is, line);
    is.clear();
    is.seekg(pos);
    is.setstate(state);
    return line;
}

/**
 * Split string based on a character delimiter.
 * @see https://stackoverflow.com/a/46931770
 */
inline void split(const std::string &str, const char delim, std::vector<std::string> &dst) {
    std::stringstream ss(str);
    for (std::string item; getline(ss, item, delim);) dst.push_back(item);
}
