#include "../include/H2D.h"
#include "../params/params.h"
#include <time.h>

static int printParam(sConst const*const psp);

static int printParam(sConst const*const psp)
{
    fprintf(stdout,"Nx= %d\n", psp->sSize->iSize1);
    fprintf(stdout,"Ny= %d\n", psp->sSize->iSize2);
    fprintf(stdout,"Npas= %d\n", psp->Tmax);
    fprintf(stdout,"Nout= %d\n", psp->TIO);
    fprintf(stdout,"C= %f\n", psp->dEta);   
    fprintf(stdout,"Dx= %f\n", psp->sSteps->dDx1);
    fprintf(stdout,"Dy= %f\n", psp->sSteps->dDx2);
    fprintf(stdout,"Dt= %f\n", psp->dDt);

    return 0;
}

int main(int argc, char **argv)
{
    sConst sParams={0.,0.,0L,0L,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    long i,j,TpsIO,TpsEnd;
    char cBuffer[1024];
    double  *Un=NULL, *Unp1=NULL;
    FILE *file=NULL;
    pfstep pfTimeStep=NULL;
    pfBC   pfBoundary=NULL;
    pfLinearOp pfRHS=NULL;
    clock_t startt = clock();
    clock_t endt = clock();
    double etime = 0.;
    
    /*initialisation of variables and profiles*/
    getParams(&sParams);

    Un   = allocArray1D(sParams.sSize->iSize1*sParams.sSize->iSize2*sParams.sSize->iSizeC,sizeof(*Un));
    Unp1 = allocArray1D(sParams.sSize->iSize1*sParams.sSize->iSize2*sParams.sSize->iSizeC,sizeof(*Unp1));

    calcMatInit(Un,sParams.sSize,sParams.sSteps);
    /* initial profiles outputs */
    GenerateFileName("H2D_OCT_",cBuffer);
    writeProfil(cBuffer,Un,sParams.sSize,_OCT);
    GenerateFileName("H2D_GPLOT_",cBuffer);
    writeProfil(cBuffer,Un,sParams.sSize,_GPLOT);

    /*time loop*/
    TpsEnd=sParams.Tmax;
    TpsIO=sParams.TIO;

    if (strcmp(scheme,"eule")==0)
    {
        pfTimeStep = eule;

    }
    else if (strcmp(scheme,"euli")==0)
    {
        pfTimeStep = euli;
    }
    else if (strcmp(scheme,"eulis")==0)
    {
        pfTimeStep = eulis;
    }
    else if (strcmp(scheme,"rk4")==0)
    {
        pfTimeStep = RK4;
    }
    else if (strcmp(scheme,"cn")==0)
    {
        pfTimeStep = cranck;
    }

    if (strcmp(discret,"fourier")==0)
    {
        pfBoundary=fourier_bc;
        pfRHS=stepDiff_fourier;

    }
    else if (strcmp(discret,"real")==0)
    {
        pfBoundary=real_bc;
        pfRHS=stepDiff_real;
        if (strcmp(scheme,"cn")==0)
        {
            pfRHS=stepDiff_real_cn;
        }
    }


if (strcmp(_boundary,"null")==0)
    {
        pfBoundary=null_bc;
    }

    startt = clock();
    for(i=0;i<TpsEnd;i+=TpsIO)
    {
        for(j=0;j<TpsIO;j++)
        {
            /* time step */
            pfTimeStep(Unp1,Un,&sParams,pfRHS,pfBoundary);            

            /* Unp1 becomes Un for next step*/
            swapFields(&Un,&Unp1);
        }
        /*data are saved each Nout iterations */        
        GenerateFileName("H2D_OCT_",cBuffer);
        writeProfil(cBuffer,Unp1,sParams.sSize,_OCT);
        GenerateFileName("H2D_GPLOT_",cBuffer);
        writeProfil(cBuffer,Unp1,sParams.sSize,_GPLOT);
    }

    /*IO*/

    endt = clock();
    etime = (float)(endt - startt) / CLOCKS_PER_SEC;
    fprintf(stdout, "Elapsed time in H2D %f s\n", etime);
    printParam(&sParams);   



    /*garbage collector*/
    freeArray1D(Un);
    freeArray1D(Unp1);
    freeParams(&sParams);

    return 0;
}

