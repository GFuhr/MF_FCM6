#include "../include/H2D_types.h"
#include "../include/H2D_Func.h"
#include "../params/params.h"


static int calcSourceInit(double *const pMatInit, sFieldSize const*const sFs, sFieldDiscret const*const sFd);

int getParams(sConst *const psc)
{
    long i=0;
    psc->Tmax = ITER;
    psc->TIO = psc->Tmax/20;
    psc->dDt           = DT;
    psc->dEta          = C;
    psc->sSize = MYLIB_MALLOC(sizeof(*psc->sSize));
    psc->sSteps = MYLIB_MALLOC(sizeof(*psc->sSteps));

    psc->sSize->iSize1 = NX;
    psc->sSize->iSize2 = NY;
    psc->sSteps->dDx1  = LX/(double)(psc->sSize->iSize1-2);
    psc->sSteps->dDx2  = LY/(double)(psc->sSize->iSize2-2);
    psc->sSteps->dKx1  = 2.*M_PI/(double)(LX);
    psc->sSteps->dKx2  = 2.*M_PI/(double)(LY);

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

    psc->pUrhs= allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pUrhs));
    psc->pUtmp= allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pUtmp));
    psc->pUk1 = allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pUk1));
    psc->pUk2 = allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pUk2));
    psc->pUk3 = allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pUk3));
    psc->pUk4 = allocArray1D((size_t)psc->sSize->iSize1*psc->sSize->iSize2*psc->sSize->iSizeC,sizeof(*psc->pUk4));

    psc->sInvX = allocArray1D(1,sizeof(*psc->sInvX));
    psc->sInvY = allocArray1D(1,sizeof(*psc->sInvY));
    psc->sSorMat = allocArray1D(1,sizeof(*psc->sSorMat));
    mallocInvLapStruct(psc->sInvX,psc->sSize);
    mallocInvLapStruct(psc->sInvY,psc->sSize);
    mallocSORStruct(psc->sSorMat,psc->sSize);

    for(i=0;i<psc->sSize->iSize2;i++)
    {
        psc->pdMKy[i]=(double)i*psc->sSteps->dKx2;
    }


    calcMatInit(psc->pInit,psc->sSize,psc->sSteps);
    calcSourceInit(psc->pdSource,psc->sSize,psc->sSteps);
    if (strcmp(discret,"fourier")==0)
    {
        initInvLapStructXF(psc->sInvX,psc->sSize,psc->sSteps->dDx1,psc->sSteps->dKx2, - psc->dEta*psc->dDt );
    }
    else
    {
        initInvLapStructX(psc->sInvX,psc->sSize,psc->sSteps->dDx1, - 0.5*psc->dEta*psc->dDt);
        initInvLapStructY(psc->sInvY,psc->sSize,psc->sSteps->dDx2, - 0.5*psc->dEta*psc->dDt);
        initSOR(psc->sSorMat,psc,-1.*psc->dEta*psc->dDt);
    }

    writeProfil("H2D_OCT_source.dat",psc->pdSource,psc->sSize,_OCT);
    writeProfil("H2D_OCT_CI.dat",psc->pInit,psc->sSize,_OCT);
    writeProfil("H2D_GPLOT_source.dat",psc->pdSource,psc->sSize,_GPLOT);
    writeProfil("H2D_GPLOT_CI.dat",psc->pInit,psc->sSize,_GPLOT);
    return 0;

}

void calcMatInit(double *const pMatInit, sFieldSize const*const sFs, sFieldDiscret const*const sFd)
{
    long ix=0,iy=0;
    int ic=0;
    double x=0;
    double y=0;

    for(ic=0;ic<sFs->iSizeC;ic++)
    {
        for(iy=0;iy<sFs->iSize2;iy++)
        {
            long offset = sFs->iSize1*(iy+sFs->iSize2*ic);
            for(ix=0;ix<sFs->iSize1;ix++)
            {
                x=(double)(ix-1)*sFd->dDx1;
                if (strcmp(discret,"fourier")==0)
                {
                    y=(double)iy*sFd->dKx2;
                }
                else
                {
                    y=(double)iy*sFd->dDx2;
                }
                pMatInit[ix+offset]=FuncInit(x,y, ic);
            }
        }
    }

}


static int calcSourceInit(double *const pMatInit, sFieldSize const*const sFs, sFieldDiscret const*const sFd)
{
    long ix=0,iy=0;
    int ic=0;
    double x,y;

    for(ic=0;ic<sFs->iSizeC;ic++)
    {
        for(iy=0;iy<sFs->iSize2;iy++)
        {
            long offset = sFs->iSize1*(iy+sFs->iSize2*ic);
            for(ix=0;ix<sFs->iSize1;ix++)
            {
                x=(double)(ix-1)*sFd->dDx1;
                if (strcmp(discret,"fourier")==0)
                {
                    y=(double)iy*sFd->dKx2;
                }
                else
                {
                    y=(double)iy*sFd->dDx2;
                }
                    pMatInit[ix+offset]=source(x,y, ic);


            }
        }
    }
        return 0;
}



/* ecriture du profil dans un fichier */
int writeProfil(const char *const cFileName, const double *const u, sFieldSize const*const sFs,H2D_IO_FORMAT format)
{
    FILE *file=NULL;
    long x,y,z;

    file=fopen(cFileName,"w");
    if (format==_GPLOT)
    {

    
    for(z=0;z<sFs->iSizeC;z++)
    {
        for(y=0;y<sFs->iSize2;y++)
        {
            for(x=0;x<sFs->iSize1;x++)
            {
                long const pos = x +y*sFs->iSize1+z*sFs->iSize1*sFs->iSize2;
                fprintf(file,"%lf\t%lf\t%lf\n",(double)x,(double)(y+z*sFs->iSize1),u[pos]);
            }
            /*fprintf(file,"\n");*/
        }
    }
    }
    else
    {
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
    }

    fclose(file),file=NULL;

    return 0;
}

void freeParams(sConst *psc)
{
    freeInvLapStruct(psc->sInvX);
    freeInvLapStruct(psc->sInvY);
    freeSORStruct(psc->sSorMat);
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

    freeArray1D(psc->sInvX);
    freeArray1D(psc->sInvY);
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