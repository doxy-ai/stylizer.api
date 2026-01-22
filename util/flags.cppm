/**
 * @file
 * @brief Defines utility functions for managing flags.
 */
export module stylizer.flags;

import magic_enum;

namespace stylizer::graphics {

	/**
	* @brief Inline namespace containing bitwise operators for flag manipulation.
	*
	* This namespace provides convenient access to bitwise operators for working with flags.
	*/
	inline namespace operators {
		/**
		* @brief Uses the magic_enum library's bitwise operators.
		*/
		using namespace magic_enum::bitwise_operators;
	}

	/**
	* @brief Checks if a given set of flags are set within a full set of flags.
	*
	* This function determines if the flags specified by the `flags` argument are present within the
	* flags represented by the `full` argument.  It uses bitwise AND to determine if the flags are set.
	*
	* @tparam T The type of the flag values.
	* @param full The full set of flags.
	* @param flags The flags to check for.
	* @return True if the flags are set, false otherwise.
	*/
	template<typename T>
	bool flags_set(T full, T flags) {
		constexpr T invalid = {};
		return (full & flags) != invalid;
	}

} // namespace stylizer::graphics

namespace stylizer {
    /**
     * @brief Re-exports the `flags_set` function from the `stylizer::graphics` namespace.
     *
     * This allows the `flags_set` function to be used directly within the `stylizer` namespace
     * without requiring explicit qualification.
     */
    using stylizer::graphics::flags_set;
}
