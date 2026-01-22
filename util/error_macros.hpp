#pragma once

#include "config.hpp"

#ifndef STYLIZER_NO_EXCEPTIONS
    #define STYLIZER_API_THROW(x) (stylizer::get_error_handler()(stylizer::error_severity::Error, (x), 0), throw error(x))
#else
    #define STYLIZER_API_THROW(x) (stylizer::get_error_handler()(stylizer::error_severity::Error, (x), 0), assert(false))
#endif