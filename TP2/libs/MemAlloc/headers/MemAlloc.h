/**
*\file MemAlloc.h
*\author G. Fuhr
*\version 1.0
*\date octobre 2008
*
* routines d'allocation/liberation de memoires
*
*/

#ifdef __cplusplus
 extern "C" {
#endif

#ifndef __MEMALLOC_H
#define __MEMALLOC_H
#include "../../C_Norm/C_Norm.h"
#include "../../C_Norm/commonhead.h"
#include "./alloc.h"
#ifdef PREDEF_STANDARD_C_1999
#include<complex.h>
#ifndef _Complex_I 
#define _Complex_I  csqrt(-1)
#endif
#endif



void *allocArray1D(size_t const iSize1,size_t const iSizeEl);
void **allocArray2D(size_t const iSize1,size_t const iSize2, size_t const dataType);
void ***allocArray3D(size_t const iSize1,size_t const iSize2,size_t const iSize3, size_t const sizeEl);
void ****allocArray4D(size_t const iSize1,size_t const iSize2,size_t const iSize3,size_t const iSize4, size_t const sizeEl);

void freeArray1D(void *pvec);
void freeArray2D(void **pvec);
void freeArray3D(void ***pvec);
void freeArray4D(void ****pvec);

#endif

#ifdef __cplusplus
}
#endif
