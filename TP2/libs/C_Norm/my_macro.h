#ifdef __cplusplus
extern "C" {
#endif

#ifndef _MYMACRO_H
#define _MYMACRO_H

#if defined(__GNUC__)
#define DO_PRAGMA(x) #warning (#x)
#define COMPILMSG(x) DO_PRAGMA("__FILE__(__LINE__) : " #x))
#elif defined(__PGI)
#define DO_PRAGMA(x) #pragma (#x)
#define COMPILMSG(x) DO_PRAGMA("__FILE__(__LINE__) : " #x))
#elif defined(_MSC_VER)
/*#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC__ __FILE__ "("__STR1__(__LINE__)") : Warning Msg: "
#define DO_PRAGMA(x) #pragma (#x)
#define COMPILMSG(x) #DO_PRAGMA(message(__LOC__#x))
*/
#elif defined(__SUNPRO_CC)

#elif defined(__ICC)
/*#define DO_PRAGMA(x) _Pragma (#x)
#define COMPILMSG(x) DO_PRAGMA(message( __FILE__"("__LINE__") : " #x))*/
#enfif

#endif _MYMACRO_H

#ifdef __cplusplus
}
#endif