#ifdef __cplusplus
extern "C" {
#endif

#ifndef _COMMON_H
#define _COMMON_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*#include "./config.h"*/
#include "./libraries.h"
/*#include "../libraries/C_Norm/C_Norm.h"*/

#ifdef H2D_USE_OMP    
    #include <omp.h>
#endif

#if defined(__ICC) || defined(__INTEL_COMPILER) 
    #include <mathimf.h>
#else
    #include <math.h>
#endif

#if defined(USED_ALIGN_MALLOC)
    #include "../libraries/MemAlloc/headers/alloc.h"
    #include "../libraries/MemAlloc/headers/MemAlloc.h"
#define H2D_FREE MYLIB_FREE
#define H2D_MALLOC MYLIB_MALLOC
#else
    #include <malloc.h>
    #undef H2D_FREE
    #undef H2D_MALLOC
    #define H2D_FREE free
    #define H2D_MALLOC malloc
#endif

#endif


#ifdef __cplusplus
}
#endif
