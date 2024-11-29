#include <concepts>
#include <utility>

namespace stylizer {
	template<typename T>
	concept releasable = requires(T t) {
		{t.release()};
	};

	template<typename T>
	struct auto_release: public T {
		auto_release() : T() {}
		auto_release(T&& raw) : T(std::move(raw)) {}
		auto_release(const auto_release&) = delete;
		auto_release(auto_release&& other) : T(std::move(other)) {}
		auto_release& operator=(T&& raw) { static_cast<T&>(*this) = std::move(raw); return *this; }
		auto_release& operator=(const auto_release&) = delete;
		auto_release& operator=(auto_release&& other) {{ static_cast<T&>(*this) = std::move(static_cast<T&>(other)); return *this; }}

		~auto_release() { if(*this) this->release(); }
	};

#define STYLIZER_API_GENERIC_AUTO_RELEASE_SUPPORT(type)\
		type() {}\
		type(const type&) = default;\
		type& operator=(const type&) = default;
}

namespace stylizer::api {
	struct device {
		struct create_config {
			bool high_performance = true;
			struct surface* surface = nullptr; // When set to null the device will be configured in headless mode
		};
	};

	template<typename T>
	concept device_concept = requires(T t, device::create_config config) {
		{ T::create_default(config) } -> std::convertible_to<T>;
	};
}