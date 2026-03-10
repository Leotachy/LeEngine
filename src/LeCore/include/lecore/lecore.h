
#pragma once

// system headers

#include <cstdio> 
#include <math.h>
#include <string.h>
#include <float.h>
#include <ctime>
#include <fstream>
#include <vector>


// third party headers

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
#define LeFree(p) (free(p))

#define STRINGIFY(x) #x
#define ARRAY_SIZE(x) sizeof(x)/(sizeof(x[0]))

// ledebug

enum LeDebugType
{
    leNADEBUGTYPE = 0,
    leDEBUGASSERT,
    leDEBUGERROR,
    leDEBUGWARN,
    leDEBUGMESSAGE
};


typedef void (*LeDebugHandler)(LeDebugType type, const LeChar* string);

void _LeDebugSendMessage(LeDebugType type, const LeChar* file, const int32 line, const LeChar* message, ...);

#define LeDebugSendMessage(type, message, ...)          \
        _LeDebugSendMessage(type,                       \
							__FILE__,			        \
                            __LINE__,                   \
                            message, __VA_ARGS__)

LeDebugHandler LeDebugSetHandler(LeDebugHandler handler);



// shortcut macros

#ifdef LE_DEBUG

#define LE_DEBUG_ASSERT(x) \
        do { \
            if (!(x)) _LeDebugSendMessage( \
                leDEBUGASSERT, \
                __FILE__, \
                __LINE__, \
                #x); \
        } while(0)

#define LE_DEBUG_ERROR(x, ...) _LeDebugSendMessage(leDEBUGERROR,__FILE__,__LINE__,x,__VA_ARGS__)
#define LE_DEBUG_WARN(x, ...) _LeDebugSendMessage(leDEBUGWARN,__FILE__,__LINE__,x,__VA_ARGS__)
#define LE_DEBUG_MESSAGE(x, ...) _LeDebugSendMessage(leDEBUGMESSAGE,__FILE__,__LINE__,x,__VA_ARGS__)

#else


#define LE_DEBUG_ASSERT(ignore)
#define LE_DEBUG_ERROR(ignore, ...)
#define LE_DEBUG_WARN(ignore, ...)
#define LE_DEBUG_MESSAGE(ignore, ...)
#define LE_DEBUG_MESSAGE_RAW(x, ...)

#endif

#define LE_ASSERT(x) \
        do { \
            if (!(x)) _LeDebugSendMessage( \
                leDEBUGASSERT, \
                __FILE__, \
                __LINE__, \
                #x); \
        } while(0)

#define LE_ERROR(x, ...) _LeDebugSendMessage(leDEBUGERROR,__FILE__, __LINE__,x,__VA_ARGS__) 
#define LE_WARN(x, ...) _LeDebugSendMessage(leDEBUGWARN,__FILE__, __LINE__,x,__VA_ARGS__) 



