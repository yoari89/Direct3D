#pragma once
#include <Windows.h>
#include <list>
#include <vector>
#include <string>
#include <d2d1.h>
#include <conio.h>
#include <dwrite.h>
#include <assert.h>
#include <process.h>
#include <functional>
#include <unordered_map>
#include <crtdbg.h>

using namespace std;

// Shader
#include <d3d11.h>
#include <d3dcompiler.h>

// Library
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")
#pragma comment(lib, "fmod_vc")

// Math
#include <PCHMath.h>

// Base
#include "Base/PreProcessor.h"

// Utility
#include "Utility/DefineMacro.h"
#include "Utility/EnumFlag.h"
#include "Utility/Types.h"