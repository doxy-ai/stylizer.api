/**
 * @file
 * @brief Defines a generic auto-release mechanism for types.
 */

#pragma once
#include <concepts>
#include <utility>

namespace stylizer {

	/**
	* @brief Concept that defines a type with a `release()` method.
	*
	*  A type `T` satisfies this concept if it has a public member function `release()`.
	*/
	template<typename T>
	concept releasable = requires(T t) {
		{ t.release() };
	};

	template<typename T>
	struct auto_release {
		static_assert(false, "Auto Release has not been configured for this type");
	};

	/**
	* @brief A template class that automatically releases a given type when it goes out of scope.
	*
	* This class provides automatic resource management by calling the `release()` method of the
	* underlying type when the `auto_release` object is destroyed.
	*
	* @note The semantics of an auto_release are very similar to those of a std::unique_ptr
	*
	* @tparam T The type to be automatically released.
	*/
	template<releasable T>
	struct auto_release<T> : public T {
		/**
		* @brief Default constructor.
		*/
		auto_release(): T() { }
		/**
		* @brief Constructor that takes a move-only argument.
		*
		* @param raw The move-only argument.
		*/
		auto_release(T&& raw): T(std::move(raw)) { }
		/**
		* @brief Deleted copy constructor.
		*/
		auto_release(const auto_release&) = delete;
		/**
		* @brief Move constructor.
		*
		* @param other The other `auto_release` object.
		*/
		auto_release(auto_release&& other): T(std::move(other)) { }
		/**
		* @brief Move assignment operator.
		*
		* @param raw The move-only argument.
		* @return A reference to this `auto_release` object.
		*/
		auto_release& operator=(T&& raw) {
			static_cast<T&>(*this) = std::move(raw);
			return *this;
		}
		/**
		* @brief Deleted copy assignment operator.
		*
		* Prevents the assignment of an `auto_release` object to another.
		*/
		auto_release& operator=(const auto_release&) = delete;
		/**
		* @brief Move assignment operator.
		*
		* @param other The other `auto_release` object.
		* @return A reference to this `auto_release` object.
		*/
		auto_release& operator=(auto_release&& other) {
			static_cast<T&>(*this) = std::move(static_cast<T&>(other));
			return *this;
		}

		/**
		* @brief Destructor.  Automatically calls the `release()` method of the underlying type.
		*/
		~auto_release() {
			if (*this) this->release();
		}
	};

	/**
	* Deduction guide to ensure releasable types will auto convert.
	*/
	template<releasable T>
	auto_release(T) -> auto_release<T>;

} // namespace stylizer
