#include "../include/H2D_types.h"
#include "../include/H2D_Func.h"
#include "../params/params.h"

 
static int calcSourceInit(double *const pMatInit, sFieldSize const*const sFs, sFieldDiscret const*const sFd);

int getParams(sConst *const psc)
{
    long i=0;
    long j=0;
    psc->Tmax = ITER;
    psc->TIO = psc->Tmax/10;
    psc->dDt           = DT;
    psc->dEta          = C;
    psc->sSize = MYLIB_MALLOC(sizeof(*psc->sSize));
    psc->sSteps = MYLIB_MALLOC(sizeof(*psc->sSteps));
    
    psc->sSize->iSize1 = NX;
    psc->sSize->iSize2 = NY;
    psc->sSteps->dDx1  = LX/(long)(psc->sSize->iSize1-2);
    psc->sSteps->dDx2  = LY/(long)(psc->sSize->iSize2-2);
    psc->sSteps->dKx1  = 2.*M_PI/(double)LX;
    psc->sSteps->dKx2  = 2.*M_PI/(double)LY;

    /*complexe ou reels depends du schema utilise*/
    if (strcmp(discret,"fourier")==0)
    {
        psc->sSize->iSizeC = 2;

    }
    else if (strcmp(discret,"real")==0)
    {
        psc->sSize->iSizeC = 1;

    }

    psc->pdSource=allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pdSource));
    psc->pInit=allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pInit));
    psc->pdMKy=allocArray1D((size_t)psc->sSize->iSize2,sizeof(*psc->pdMKy));

    psc->pUrhs=allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pUrhs));
    psc->pUtmp=allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pUtmp));
    psc->pUk1=allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pUk1));
    psc->pUk2=allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pUk2));
    psc->pUk3=allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pUk3));
    psc->pUk4=allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pUk4));

    for(i=0;i<psc->sSize->iSize2;i++)
    {
        psc->pdMKy[i]=(double)i*psc->sSteps->dKx2;
    }

    calcMatInit(psc->pInit,psc->sSize,psc->sSteps);
    calcSourceInit(psc->pdSource,psc->sSize,psc->sSteps);
    
}

 void calcMatInit(double *const pMatInit, sFieldSize const*const sFs, sFieldDiscret const*const sFd)
{
    long ix=0,iy=0,ic=0;
    long const fLen=sFs->iSize1*sFs->iSize2*sFs->iSizeC;

    for(ic=0;ic<sFs->iSizeC;ic++)
    {
        for(iy=0;iy<sFs->iSize2;iy++)
        {
            long offset = sFs->iSize1*(iy+sFs->iSize2*ic);
            for(ix=0;ix<sFs->iSize1;ix++)
            {
                if (strcmp(discret,"fourier")==0)
                {
                    pMatInit[ix+offset]=FuncInit(ix*sFd->dDx1,iy*sFd->dKx2, ic);
                }
                else
                {
                    pMatInit[ix+offset]=FuncInit(ix*sFd->dDx1,iy*sFd->dDx2, ic);
                }
            }
        }
    }
}


static int calcSourceInit(double *const pMatInit, sFieldSize const*const sFs, sFieldDiscret const*const sFd)
{
    long ix=0,iy=0,ic=0;
    long const fLen=sFs->iSize1*sFs->iSize2*sFs->iSizeC;

    for(ic=0;ic<sFs->iSizeC;ic++)
    {
        for(iy=0;iy<sFs->iSize2;iy++)
        {
            long offset = sFs->iSize1*(iy+sFs->iSize2*ic);
            for(ix=0;ix<sFs->iSize1;ix++)
            {
                if (strcmp(discret,"fourier")==0)
                {
                    pMatInit[ix+offset]=source(ix*sFd->dDx1,iy*sFd->dDx2, ic);
                }
                else
                {
                    pMatInit[ix+offset]=source(ix*sFd->dDx1,iy*sFd->dKx2, ic);
                }
                
            }
        }
    }
}



/* ecriture du profil dans un fichier */
int writeProfil(const char *const cFileName, const double *const u, sFieldSize const*const sFs)
{
    FILE *file=NULL;
    long x,y,z;

    file=fopen(cFileName,"a");
    for(z=0;z<sFs->iSizeC;z++)
    {
        for(y=0;y<sFs->iSize2;y++)
        {
            for(x=0;x<sFs->iSize1;x++)
            {
                long const pos = x +y*sFs->iSize1+z*sFs->iSize1*sFs->iSize2;
                fprintf(file,"%lf\t",u[pos]);
            }
            fprintf(file,"\n");
        }
    }
    
    fclose(file),file=NULL;

    return 0;
}

void freeParams(sConst *psc)
{
    freeArray1D(psc->sSize);     /*! <  taille globale du champs */
    freeArray1D(psc->sSteps);      /*! < valeur des pas dx,ky,dz dans l'espace de fourier */

    freeArray1D(psc->pdSource);                  /*! < champ 1D contenant le profil de source*/

    freeArray1D(psc->pInit);                  /*! < champ 1D contenant le profil de source*/

    freeArray1D(psc->pdMKy);                      /*! < tableau 1D contenant les valeur de m*ky pour O<m<Mmax */


    freeArray1D(psc->pUrhs);
    freeArray1D(psc->pUtmp);

    freeArray1D(psc->pUk1);
    freeArray1D(psc->pUk2);
    freeArray1D(psc->pUk3);
    freeArray1D(psc->pUk4);

    freeArray1D(psc->sInvW);
}
/*

void initParams(sConst *psc)
{
    freeArray1D(psc->sSize);    
    freeArray1D(psc->sSteps);      

    freeArray1D(psc->pdSource);                 

    freeArray1D(psc->pInit);                 

    freeArray1D(psc->pdMKy);                  


    freeArray1D(psc->pUrhs);
    freeArray1D(psc->pUtmp);

    freeArray1D(psc->pUk1);
    freeArray1D(psc->pUk2);
    freeArray1D(psc->pUk3);
    freeArray1D(psc->pUk4);

    freeArray1D(psc->sInvW);;

}
*/