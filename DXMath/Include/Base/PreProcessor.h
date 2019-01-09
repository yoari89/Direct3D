#pragma once

#ifdef MATH_EXPORT
#define MATH_DLL	__declspec(dllexport)
#define MATH_ALIGN	__declspec(align(16))
#else
#define MATH_DLL	__declspec(dllimport)
#define MATH_ALIGN	__declspec(align(16))
#endif // MATH_EXPORT