#include "../include/H2D_Func.h"
#include "../include/schemes.h"
#include "../include/H2D_types.h"

int eule(double *const Up1, double const*const U, sConst const*const psc,pfLinearOp pfTimeStep, pfBC pfBoundary)
{

    pfTimeStep(psc->pUrhs,U,psc);
    addfields(Up1,U,psc->pUrhs,psc->dDt,psc->sSize);
    pfBoundary(Up1,psc->sSize);

    return 0;
}

