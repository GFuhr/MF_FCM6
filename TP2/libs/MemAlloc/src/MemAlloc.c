/**
*\file MemAlloc.c
*\author G. Fuhr
*\version 1.0
*\date octobre 2008
*
* routines pour le choix des fonctions a utiliser pour les allocations/liberations de memoire
*
*/

#include "../headers/alloc.h"
#include "../headers/MemAlloc.h"

/*#define RBM_DEBUG_CALL*/

/**
*\fn void *allocArray1D(size_t const iNumEl, size_t const iSizeEl)
*\brief allocation de memoire pour des tableaux 1D
*\param iNumEl : nombre d'elements
*\param iSizeEl : taille d'un element
*\return : pointeur vers le tableau alloue un NULL en cas d'erreur
*/
void *allocArray1D(size_t const iNumEl, size_t const iSizeEl)
{
    size_t const malloc_size = iNumEl*iSizeEl;
    size_t i=0;
    char *p=MYLIB_MALLOC(malloc_size);



    if (p==NULL)
        return NULL;

    for(i=0;i<malloc_size;i++)
    {
        p[i]='\0';
    }
    return (void *)p;

}


/**
*\fn void **allocArray2D(size_t const iSize1,size_t const iSize2, size_t const dataType)
*\brief allocation de memoire pour des tableaux 2D sous la forme tab[Size1][Size2]
*\param iSize1 : nombre d'elements dans la direction 1
*\param iSize2 : nombre d'elements dans la direction 2
*\param dataType : taille d'un element
*\return : pointeur vers le tableau alloue un NULL en cas d'erreur
*/
void **allocArray2D(size_t const iSize1,size_t const iSize2, size_t const dataType)
{
    static const char FCNAME[] = "allocArray2D";
    char ** pf;
    size_t i=0;
    /*size_t sizeEl=getSizeType(dataType);
    union mem_data_type_2 pDataPoint;
    if (sizeEl==0)
    return NULL;*/


    pf= MYLIB_MALLOC(iSize1*sizeof(&i));


    if (pf==NULL)
        return NULL;

    pf[0]= MYLIB_MALLOC(iSize1*iSize2*dataType);

    if (pf[0]==NULL)
    { 
        MYLIB_FREE(pf),pf=NULL;
        return NULL;
    }

    for(i=1;i<iSize1;i++)
        pf[i]=(pf[0]+i*iSize2*dataType);


    for(i=0;i<iSize1*iSize2*dataType;i++)
    {
        pf[0][i]='\0';
    }

    return (void **)pf;
}



void freeArray2D(void **pVec)
{
    static const char FCNAME[] = "freeArray2D";
    if (pVec!=NULL)
    {
        MYLIB_FREE(pVec[0]);
        MYLIB_FREE(pVec);
    }
}

void freeArray1D(void *pVec)
{
    static const char FCNAME[] = "freeArray1D";
    


    if (pVec!=NULL)
    {
        MYLIB_FREE(pVec);
    }
}


