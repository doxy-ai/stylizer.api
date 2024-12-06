#pragma once

#include <span>

template<typename T>
std::span<T> span_from_value(const T& value) {
    return {(T*)&value, 1};
}

template<typename T>
std::span<std::byte> byte_span(std::span<T> span) { return {(std::byte*)span.data(), span.size_bytes()}; }