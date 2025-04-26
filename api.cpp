#include "api.hpp"

namespace stylizer {
	stylizer::signal<void(error_severity, std::string_view message, size_t error_tag)>& get_error_handler() {
		static stylizer::signal<void(error_severity, std::string_view message, size_t error_tag)> handler;
		return handler;
	}
}