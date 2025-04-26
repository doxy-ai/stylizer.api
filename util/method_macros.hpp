/**
* Macro used to define some method in the base (api::) classes which support C interfaces.
*
* @note None of these are intended to be directly used... bindings to other languages may find them helpful however
*/
#define STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_METHOD(type)       \
	template<typename T>                                       \
	static type* move_temporary_to_heap_impl(T&& value) {      \
		return (type*)(new T(std::move(value)));               \
	}                                                          \
	virtual type* move_temporary_to_heap(type& temporary) = 0; \
	type* move_temporary_to_heap() { return move_temporary_to_heap(*this); }

/**
* Macro used to define some method in the derived classes which support C interfaces.
*
* @note None of these are intended to be directly used... bindings to other languages may find them helpful however
*/
#define STYLIZER_API_MOVE_TEMPORARY_TO_HEAP_DERIVED_METHOD(type)                    \
	type&& move() { return std::move(*this); }                                      \
	api::type* move_temporary_to_heap(api::type& temporary) override {              \
		return api::type::move_temporary_to_heap_impl(temporary.as<type>().move()); \
	}

/**
* @brief Macro used to generate default constructors and copy constructors for types.
*/
#define STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(type) \
	type() { }                                          \
	type(const type&) = default;                        \
	type& operator=(const type&) = default;

/**
* @brief Macro used to define the `as()` method for dynamic casting.
*
* The `as()` method allows for safe downcasting to derived types.
*
* @code
*   MyDerivedClass* derived = my_auto_release.as<MyDerivedClass>();
* @endcode
*/
#ifndef STYLIZER_NO_RTTI
	#define STYLIZER_API_AS_METHOD(type)                 \
		template<std::derived_from<type> T>              \
		T& as() {                                        \
			T* out = dynamic_cast<T*>(this);             \
			assert(out != nullptr);                      \
			return *out;                                 \
		}                                                \
		template<std::derived_from<type> T>              \
		const T& as() const {                            \
			const T* out = dynamic_cast<const T*>(this); \
			assert(out != nullptr);                      \
			return *out;                                 \
		}
#else
	#define STYLIZER_API_AS_METHOD(type)    \
		template<std::derived_from<type> T> \
		T& as() { return *(T*)this; }       \
		template<std::derived_from<type> T> \
		const T& as() const { return *(const T*)this; }
#endif