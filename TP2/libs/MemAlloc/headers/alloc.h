/**
*\file MemAlloc/headers/alloc.h
*\author G. Fuhr
*\version 1.0
*\date octobre 2008
*
* routines pour le choix des fonctions a utiliser pour les allocations/liberations de memoire
*
*/


#ifdef __cplusplus
 extern "C" {
#endif

#ifndef __MALLOC_H__
    #define __MALLOC_H__

#define MYLIB_HAVE_MEMALIGN

    #include <stdlib.h>        /* size_t */


    #define MYLIB_IMPOSED_ALIGNMENT 16


    
    #if (defined(_XOPEN_SOURCE)) 
        #if (_XOPEN_SOURCE - 0) >= 600
            #define MYLIB_HAVE_POSIX_MEMALIGN
            #undef MYLIB_HAVE_MEMALIGN
        #else
            #define MYLIB_HAVE_MEMALIGN
            #undef MYLIB_HAVE_POSIX_MEMALIGN
        #endif
    #elif (defined(_GNU_SOURCE)|| defined (__USE_XOPEN2K))
/*        #pragma warning(message,"ou encore la")    */
        #define MYLIB_HAVE_POSIX_MEMALIGN
        #undef MYLIB_HAVE_MEMALIGN
    #else
        #undef MYLIB_HAVE_POSIX_MEMALIGN
        #undef MYLIB_HAVE_MEMALIGN
    #endif
    
    /* determine precision and name-mangling scheme */
    #define CONCAT(prefix, name) prefix ## name
    #define X(name) CONCAT(align_, name)

/*-----------------------------------------------------------------------*/
/* alloc.c: */

    void *X(malloc)(size_t n, size_t align);
    void X(free)(void *p);

    
    #ifdef MYLIB_IMPOSED_ALIGNMENT
        #define MYLIB_MALLOC(n)  X(malloc)(n,MYLIB_IMPOSED_ALIGNMENT)
    #else
        #define MYLIB_MALLOC(n)  X(malloc)(n,16)
    #endif
    
    #define MYLIB_A_MALLOC(n,what) X(malloc)(n,what)
    
    #define MYLIB_FREE(p) X(free)(p)

#endif 

#ifdef __cplusplus
}
#endif
