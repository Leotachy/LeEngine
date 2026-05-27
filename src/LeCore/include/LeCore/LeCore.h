
#pragma once

#define SUPPORT_IMGUI

// system headers

#include <cstdio> 
#include <math.h>
#include <string.h>
#include <float.h>
#include <ctime>
#include <fstream>
#include <vector>
#include <set>
#include <queue>
#include <deque>
#include <functional>

// third party headers

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef SUPPORT_IMGUI
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#endif

// types

typedef long long int int64;
typedef long long unsigned int uint64;
typedef int int32;
typedef unsigned int uint32;
typedef short int16;
typedef unsigned short uint16;
typedef char int8;
typedef unsigned char uint8;

typedef double float64;
typedef float float32;

typedef uint8 LeByte;
typedef char LeChar;
typedef bool LeBool;



// General Support Macros
#define LeMemcpy memcpy
#define LeMemmove memmove
#define LeMemset memset
#define LeMemcmp memcmp
#define LeMalloc(size) (malloc(size))
#define LeFree(p) (free((void*)p))

#define STRINGIFY(x) #x
#define ARRAY_SIZE(x) sizeof(x)/(sizeof(x[0]))


// Extra header files that are "convenient" to have around
#include "LeDebug/LeDebug.h"
#include "LeUtils/LeHash.h"
#include "LeRGBA/LeRGBA.h"