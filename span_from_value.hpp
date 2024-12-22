#pragma once

#include <span>

template<typename T>
std::span<T> span_from_value(const T& value) {
    return {(T*)&value, 1};
}