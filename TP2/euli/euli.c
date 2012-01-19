#include "../include/H2D_types.h"
#include "../include/H2D_Func.h"
#include "../params/params.h"

int euli(double *const Up1, double const*const U, sConst const*const psc,pfLinearOp pfTimeStep, pfBC pfBoundary)
{
    if (strcmp(discret,"real")==0)
    {

    /*t->t+1/2 */
        addfields(psc->pUrhs,U,psc->pdSource,0.5*psc->dDt,psc->sSize);
        invMat(psc->pUrhs,psc->pUk1,psc->sSize,psc->sInvX);
        pfBoundary(psc->pUk1,psc->sSize);

    /*t+1/2->t+1 */
        addfields(psc->pUrhs,psc->pUk1,psc->pdSource,0.5*psc->dDt,psc->sSize);
        invMat(psc->pUrhs,Up1,psc->sSize,psc->sInvY);
        pfBoundary(Up1,psc->sSize);
    }
    else if (strcmp(discret,"fourier")==0)
    {
        addfields(psc->pUrhs,U,psc->pdSource,psc->dDt,psc->sSize);
        invMat(psc->pUrhs,Up1,psc->sSize,psc->sInvX);
        pfBoundary(Up1,psc->sSize);
    }

    return 0;
}


#include "../include/H2D_types.h"
#include "../include/H2D_Func.h"
#include "../params/params.h"

int eulis(double *const Up1, double const*const U, sConst const*const psc,pfLinearOp pfTimeStep, pfBC pfBoundary)
{
    if (strcmp(discret,"real")==0)
    {

        /*t->t+1/2 iX*/
        addfields(psc->pUrhs,U,psc->pdSource,psc->dDt,psc->sSize);
        sor(Up1,psc->pUrhs,psc->sSorMat,psc->sSize);
        pfBoundary(Up1,psc->sSize);

    }
    else if (strcmp(discret,"fourier")==0)
    {
        euli(Up1,U,psc,pfTimeStep,pfBoundary);
    }

    return 0;
}
