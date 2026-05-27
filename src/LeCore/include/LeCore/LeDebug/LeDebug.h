#pragma once


enum class LeDebugMessageType
{
    None,
    Assert,
    Error,
    Warn,
    Message
};


typedef void (*LeDebugHandler)(LeDebugMessageType type, const LeChar* string);

void _LeDebugSendMessage(LeDebugMessageType type, const LeChar* file, const int32 line, const LeChar* message, ...);

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
                LeDebugMessageType::Assert, \
                __FILE__, \
                __LINE__, \
                #x); \
        } while(0)

#define LE_DEBUG_ERROR(x, ...) _LeDebugSendMessage(LeDebugMessageType::Error,__FILE__,__LINE__,x,__VA_ARGS__)
#define LE_DEBUG_WARN(x, ...) _LeDebugSendMessage(LeDebugMessageType::Warn,__FILE__,__LINE__,x,__VA_ARGS__)
#define LE_DEBUG_MESSAGE(x, ...) _LeDebugSendMessage(LeDebugMessageType::Message,__FILE__,__LINE__,x,__VA_ARGS__)

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
                LeDebugMessageType::Assert, \
                __FILE__, \
                __LINE__, \
                #x); \
        } while(0)

#define LE_ERROR(x, ...) _LeDebugSendMessage(LeDebugMessageType::Error,__FILE__, __LINE__,x,__VA_ARGS__) 
#define LE_WARN(x, ...) _LeDebugSendMessage(LeDebugMessageType::Warn,__FILE__, __LINE__,x,__VA_ARGS__) 
