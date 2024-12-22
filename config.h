#pragma once

#ifdef __GNUC__
	#if !defined(__EXCEPTIONS) && !defined(STYLIZER_NO_EXCEPTIONS)
		#define STYLIZER_NO_EXCEPTIONS
	#endif
	#if !defined(__GXX_RTTI) && !defined(STYLIZER_NO_RTTI)
		#define STYLIZER_NO_RTTI
	#endif
#elif defined(_MSC_VER)
	#if !defined(_CPPUNWIND) && !defined(STYLIZER_NO_EXCEPTIONS)
		#define STYLIZER_NO_EXCEPTIONS
	#endif
	#if !defined(_CPPRTTI) && !defined(STYLIZER_NO_RTTI)
		#define STYLIZER_NO_RTTI
	#endif
#endif