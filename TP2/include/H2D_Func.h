
#ifndef H2D_FUNC_H
#define H2D_FUNC_H
#include "H2D_types.h"



double    source(double const x, double const y, int const dComplex);
double  FuncInit(double const x, double const y, int const dComplex);

int stepDiff_fourier(double *const pp1,double const*const p,  sConst const*const psc);
int stepDiff_real(double *const pp1,double const*const p,  sConst const*const psc);
int addfields(double *const pp1,double const*const p,double const*const  rhs, double const dFactor, sFieldSize const*const sFs);
int cleanfields(double *const pp1,double const*const p,double const*const  rhs, double const dFactor, sFieldSize const*const sFs);
void swapFields(double **f1,double **f2);

void fourier_bc(double *const field,sFieldSize const*const sFs);
void real_bc(double *const field,sFieldSize const*const sFs);

int getParams(sConst *const psc);
void freeParams(sConst *psc);
int writeProfil(const char *const cFileName, const double *const u, sFieldSize const*const sFs);
void calcMatInit(double *const pMatInit, sFieldSize const*const sFs, sFieldDiscret const*const sFd);

int RK4 (double *const Up1, double const*const U, sConst const*const psc,pfLinearOp pfTimeStep, pfBC pfBoundary);
int eule(double *const Up1, double const*const U, sConst const*const psc,pfLinearOp pfTimeStep, pfBC pfBoundary);

#endif