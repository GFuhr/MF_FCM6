#include "../include/H2D.h"
#include "../params/params.h"

int main(int argc, char **argv)
{
    sConst sParams={0.,0.,0.,0.,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
    long i,j,Tps,TpsIO,TpsEnd;
    char cBuffer[1024];
    double *Udiff=NULL, *Uadv=NULL, *Un=NULL, *Unp1=NULL,  *Utmp=NULL;
    FILE *file=NULL;
    pfstep pfTimeStep=NULL;
    pfBC   pfBoundary=NULL;
    pfLinearOp pfRHS=NULL;

    /*initialisation variables*/
    /*on recupere les informations sur la simulation, ainsi que les profils initiaux et de source */
    getParams(&sParams);

    Un   = allocArray1D(sParams.sSize->iSize1*sParams.sSize->iSize2*sParams.sSize->iSizeC,sizeof(*Un));
    Unp1 = allocArray1D(sParams.sSize->iSize1*sParams.sSize->iSize2*sParams.sSize->iSizeC,sizeof(*Unp1));

    calcMatInit(Un,sParams.sSize,sParams.sSteps);
    /* ecriture des profils initiaux */
    GenerateFileName("H2D_",cBuffer);
    file=fopen(cBuffer,"w+");
    fclose(file);
    writeProfil(cBuffer,Un,sParams.sSize);
    /*boucle temporelle*/
    TpsEnd=sParams.Tmax;
    TpsIO=sParams.TIO;

    if (strcmp(scheme,"eule")==0)
    {
        pfTimeStep = eule;

    }
    else if (strcmp(scheme,"euli")==0)
    {

    }
    else if (strcmp(scheme,"rk4")==0)
    {
        pfTimeStep = RK4;
    }
        else if (strcmp(scheme,"cn")==0)
        {

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
    }



    for(i=0;i<TpsEnd;i+=TpsIO)
    {
        for(j=0;j<TpsIO;j++)
        {
            /* avancement en temps */
            pfTimeStep(Unp1,Un,&sParams,pfRHS,pfBoundary);            

            /*pour le pas d'apres Unp1 va devenir le nouveau Un */
            swapFields(&Un,&Unp1);
        }
        /*ecriture des profils tout les Nout iterations */        
            GenerateFileName("H2D_",cBuffer);
        writeProfil(cBuffer,Unp1,sParams.sSize);
    }

    /*IO*/





    /*liberation memoire allouee*/
    freeArray1D(Un);
    freeArray1D(Unp1);
    freeParams(&sParams);

    return 0;
}