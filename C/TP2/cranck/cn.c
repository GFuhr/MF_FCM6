#include "../include/H2D_types.h"
#include "../include/H2D_Func.h"
#include "../params/params.h"

int cranck(double *const Up1, double const*const U, sConst *const psc,pfLinearOp pfTimeStep, pfBC pfBoundary)
{
    if (strcmp(discret,"real")==0)
    {
        psc->eDir =_X;
        pfTimeStep(psc->pUrhs,U,psc);
        addfields(psc->pUk1,U,psc->pUrhs,0.5*psc->dDt,psc->sSize);
        invMat(psc->pUk1,psc->pUk2,psc->sSize,psc->sInvY);
        pfBoundary(psc->pUk2,psc->sSize);

        psc->eDir =_Y;
        pfTimeStep(psc->pUrhs,psc->pUk2,psc);
        addfields(psc->pUk1,psc->pUk2,psc->pUrhs,0.5*psc->dDt,psc->sSize);
        invMat(psc->pUk1,Up1,psc->sSize,psc->sInvX);
        pfBoundary(Up1,psc->sSize);


    }
    else if (strcmp(discret,"fourier")==0)
    {
        psc->eDir =_XF;
        pfTimeStep(psc->pUrhs,U,psc);
        addfields(psc->pUk1,U,psc->pUrhs,0.5*psc->dDt,psc->sSize);
        addfields(psc->pUk1,U,psc->pdSource,0.5*psc->dDt,psc->sSize);
        invMat(psc->pUk1,Up1,psc->sSize,psc->sInvX);
        pfBoundary(Up1,psc->sSize);
    }

    return 0;
}
