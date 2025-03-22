#ifndef STYLIZER_EVENT_IMPLEMENTATION
#define STYLIZER_EVENT_IMPLEMENTATION

#include <vector>
#include <functional>

namespace stylizer {
	template<typename... Args>
	struct event: public std::vector<std::function<void(Args...)>> {
		void operator()(Args... args) {
			for(auto& f: *this) f(std::forward<Args>(args)...);
		}
	};
	template<>
	struct event<void>: public std::vector<std::function<void()>> {
		void operator()() {
			for(auto& f: *this) f();
		}
	};
}

#endif // STYLIZER_EVENT_IMPLEMENTATION