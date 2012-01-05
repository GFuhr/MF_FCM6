#ifndef MEMORY_ALLOC_H
#define MEMORY_ALLOC_H
#include "common.h"
#include "operators.h"

/*----------- Memory.c -----------*/
void * allocArray1D(size_t const size);
void ** allocArray2D(size_t const size1,size_t const size2);

void freeArray1DC(double *p);
void freeArray2DC(double **p);

/*
psRK4Vec allocRK4Vec(sFieldSize const *const sFs);
psRK4Field allocRK4Field(sFieldSize const *const sFs);
psListFields allocListField(sFieldSize const *const sFs);
psNLFields allocNLField(sFieldSize const *const sFs);
psTimeField allocTimeField(size_t const iSize1,size_t const iSize2, size_t const iSize3);
psTimeField allocTimeField_Light(size_t const iSizex,size_t const iSizey, size_t const iSizez);
void freeTimeField(psTimeField pstf);
void freeNLField(psNLFields pslf);
void freeListField(psListFields pslf);
void freeRK4Vec(psRK4Vec psrk4);
void freeRK4Field(psRK4Field psrk4);
psConst malloc_sConst(sConst *ps,sFieldSize const *const sFsSpectral);
void free_sConst(psConst structParam);
RBM_codes   malloc_sMesh(sMeshGrid *const sMesh,sFieldSize const*const sFsReal);
void free_sMesh(sMeshGrid *const sMesh);*/

#endif

