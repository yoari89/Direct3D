#pragma once

#ifdef ENGINE_EXPORT
#define ENGINE_DLL		__declspec(dllexport)
#define ENGINE_ALIGN	__declspec(align(16))
#else
#define ENGINE_DLL		__declspec(dllimport)
#define ENGINE_ALIGN	__declspec(align(16))
#endif // ENGINE_EXPORT