#include "../include/H2D_Func.h"
#include "../include/schemes.h"
#include "../include/H2D_types.h"

static void addRK4(double *const Up1,double const*const U,double const*const pUk1,double const*const pUk2,double const*const pUk3,double const*const pUk4,double const dDt,sFieldSize const*const sSize);

int RK4(double *const Up1, double const*const U, sConst const*const psc,pfLinearOp pfTimeStep, pfBC pfBoundary)
{

    /*calcul de K1*/
    pfTimeStep(psc->pUk1,U,psc);
    addfields(psc->pUtmp,U,psc->pUk1,0.5*psc->dDt,psc->sSize);
    pfBoundary(psc->pUtmp,psc->sSize);

    /*calcul de K2*/
    pfTimeStep(psc->pUk2,psc->pUtmp,psc);
    addfields(psc->pUtmp,U,psc->pUk2,0.5*psc->dDt,psc->sSize);
    pfBoundary(psc->pUtmp,psc->sSize);

    /*calcul de K3*/
    pfTimeStep(psc->pUk3,psc->pUtmp,psc);
    addfields(psc->pUtmp,U,psc->pUk3,0.5*psc->dDt,psc->sSize);
    pfBoundary(psc->pUtmp,psc->sSize);

    /*calcul de K4*/
    pfTimeStep(psc->pUk4,psc->pUtmp,psc);

    

    addRK4(Up1,U,psc->pUk1,psc->pUk2,psc->pUk3,psc->pUk4,psc->dDt,psc->sSize);
    pfBoundary(Up1,psc->sSize);

    return 0;
}

static void addRK4(double *const Up1,double const*const U,double const*const pUk1,double const*const pUk2,double const*const pUk3,double const*const pUk4,double const dDt,sFieldSize const*const sSize)
{
    long iLen=sSize->iSize1*sSize->iSize2*sSize->iSizeC;
    long i=0;
        double dFactor = dDt/6.;

        for(i=0;i<iLen;i++)
        {
            Up1[i]=U[i]+dFactor*(pUk1[i]+2*(pUk2[i]+pUk3[i])+pUk4[i]);
        }
}

