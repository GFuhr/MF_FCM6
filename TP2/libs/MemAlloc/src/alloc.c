/**
*\file alloc.c
*\author G. Fuhr
*\version 1.0
*\date octobre 2008
*
* routines pour le choix des fonctions a utiliser pour les allocations/liberations de memoire
*
*/

#include "../../C_Norm/commonhead.h"
#include "../headers/alloc.h"
#include <malloc.h>


#if defined(MYLIB_HAVE_MEMALIGN)
    #if defined(MYLIB_HAVE_MALLOC_H)

    #else

        extern void *memalign(size_t, size_t);
    #endif
#elif defined(MYLIB_HAVE_POSIX_MEMALIGN) 
    #if defined(MYLIB_HAVE_MALLOC_H)

    #else
        extern int posix_memalign(void **, size_t, size_t);
    #endif
#else
    #if defined(MYLIB_HAVE_MALLOC_H)

    #endif
#endif



#define real_malloc X(malloc)
#define real_free free /* memalign and malloc use ordinary free */



/* part of user-callable API */
void *X(malloc)(size_t n, size_t align)
{
     void *p;
 

    #if defined(MYLIB_HAVE_MEMALIGN)
        #undef real_free
        #define real_free free
             p = memalign(IMPOSED_ALIGNMENT, n);


    #elif defined(MYLIB_HAVE_POSIX_MEMALIGN)
         /* note: posix_memalign is broken in glibc 2.2.5: it constrains
            the size, not the alignment, to be (power of two) * sizeof(void*).
        The bug seems to have been fixed as of glibc 2.3.1. */

             if (posix_memalign(&p, align, n))
                  p = (void*) 0;

    #elif defined(__ICC) || defined(__INTEL_COMPILER) 
     /* Intel's C compiler defines _mm_malloc and _mm_free intrinsics */
            p = (void *) _mm_malloc(n, align);

        #undef real_free
        #define real_free _mm_free

    #elif defined(_MSC_VER)
        /* MS Visual C++ 6.0 with a "Processor Pack" supports SIMD
                and _aligned_malloc/free (uses malloc.h) */
            p = (void *) _aligned_malloc(n, align);

        #undef real_free
        #define real_free _aligned_free

    #elif (defined(__MACOSX__) || defined(__APPLE__))
             /* MacOS X malloc is already 16-byte aligned */

             p = malloc(n);
    #else
             /* Add your machine here and send a patch to fftw@fftw.org 
            or (e.g. for Windows) configure --with-our-malloc16 */
    
        p = malloc(n);
    #endif


     return p;
}



/* part of user-callable API */
void X(free)(void *p)
{
     real_free(p);
}
