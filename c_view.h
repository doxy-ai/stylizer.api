#ifdef __cplusplus
#include <cstdint>
#include <vector>
#include <span>

namespace stylizer::detail {
	template<typename T>
	struct view {
		T* data;
		size_t size;

		inline std::span<T> span() const { return {data, size}; }
		inline std::vector<T> vector() const { return std::vector<T>{data, data+size}; }

		inline operator std::span<T>() const { return span(); }
		inline operator std::vector<T>() const { return vector(); }
	};
}

#else

#include <stdint.h>

#define STYLIZER_VIEW(type) struct {\
	type* data;\
	size_t size;\
}

#endif