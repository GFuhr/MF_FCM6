#ifndef SCHEMES_H
#define SCHEMES_H

int RK4(double *const Up1, double const*const U, sConst const*const psc,pfLinearOp pfTimeStep, pfBC pfBoundary);
int eule(double *const Up1, double const*const U, sConst const*const psc,pfLinearOp pfTimeStep, pfBC pfBoundary);


#endif