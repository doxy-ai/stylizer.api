/**
 * @file
 * @brief Provides utility functions for working with spans.
 */
export module stylizer.spans;

import std.compat;

namespace stylizer {

	/**
	* @brief Creates a span containing a single element.
	*
	* This function creates a `std::span` that points to a single element of the given type.
	* It's useful for creating a span representing a single value.
	*
	* @tparam T The type of the value to create a span for.
	* @param value The value to create a span for.
	* @return A span containing a single element of the given type.
	*/
	export template<typename T>
	std::span<const T> span_from_value(const T& value) { return {(T*)&value, 1}; }
	export template<typename T>
	std::span<T> span_from_value(T& value) { return {(T*)&value, 1}; }

	/**
	* @brief Creates a byte span from a span of any type.
	*
	* This function takes a `std::span` of any type and creates a `std::span<const std::byte>`
	* that represents the same memory region as the input span, but treating it as a sequence of bytes.
	*
	* @tparam T The type of the input span.
	* @param span The input span.
	* @return A byte span representing the same memory region as the input span.
	*/
	export template<typename T>
	std::span<const std::byte> byte_span(std::span<const T> span) { return {(std::byte*)span.data(), span.size() * sizeof(T)}; }

	namespace unsafe {
		export template<typename T>
		std::span<T> span_remove_const(std::span<const T> in) {
			return std::move((std::span<T>&)in);
		}
	}

} // namespace stylizer