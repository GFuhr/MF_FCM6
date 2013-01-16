#include "../include/H2D.h"
#include "../params/params.h"

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





    /*garbage collector*/
    freeArray1D(Un);
    freeArray1D(Unp1);
    freeParams(&sParams);

    return 0;
}

