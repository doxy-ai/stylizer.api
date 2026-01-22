module;

#include "thirdparty/signals.hpp"

export module stylizer.errors;

import std.compat;
import stylizer.auto_release;

namespace stylizer {

	using namespace fteng;
	export using fteng::signal;
	export using fteng::connection;
	export using fteng::connection_raw;

	template<>
	struct auto_release<connection_raw> : public connection {
		using connection::connection;
		using connection::operator=;
	};

	export auto_release(connection_raw) -> auto_release<connection_raw>;

	export enum class error_severity {
		Verbose,
		Info,
		Warning,
		Error,
	};

	export stylizer::signal<void(error_severity, std::string_view message, size_t error_tag)>& get_error_handler() {
		static stylizer::signal<void(error_severity, std::string_view message, size_t error_tag)> handler;
		return handler;
	}

	export struct error : public std::runtime_error {
		using std::runtime_error::runtime_error;
		error(std::string_view message): std::runtime_error(std::string(message)) { }

		using severity = error_severity;
	};

}