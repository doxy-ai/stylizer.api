/**
 * @file
 * @brief Provides a function to generate a magic number from a string.
 */

#pragma once

#include <cstdint>
#include <string_view>

namespace stylizer::api {

	/**
	* @brief Generates a magic number from a string using a simple hash function.
	*
	* This function iterates through the characters of the input string and calculates a 32-bit
	* magic number.  It multiplies the current magic number by 31 and adds the ASCII value of each
	* character. This function is suitable for simple hashing needs.
	*
	* @param str The input string.
	* @return A 32-bit magic number generated from the input string.
	*/
	constexpr uint32_t string2magic(std::string_view str) {
		uint32_t magic = 0;
		for (char c : str)
			magic = (magic * 31) + c;
		return magic;
	}

} // namespace stylizer::api
