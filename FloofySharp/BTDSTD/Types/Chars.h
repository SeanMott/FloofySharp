#pragma once

//defines a common type def for chars in strings

#if defined(_WIN32)
#include <Windows.h>
#endif

//defines a not wide variant of char
typedef char char_force_not_wide_t;

//if we're on Windows we use a "wide char", and if a wide char isn't defined we just use a "unsigned short"
#ifdef _WIN32

#ifdef _WCHAR_T_DEFINED
typedef wchar_t char_t;
#else
typedef unsigned short char_t;
#endif

//if we're on any other platform it defines a uint8 and uses that as a regular char, since thoses run on Unix
#else
typedef char char_t;
#endif