
#ifndef H2D_FUNC_H
#define H2D_FUNC_H
#include "H2D_types.h"
#include "schemes.h"


double    source(double const x, double const y, int const dComplex);
double  FuncInit(double const x, double const y, int const dComplex);

int stepDiff_fourier(double *const pp1,double const*const p,  sConst const*const psc);
int stepDiff_real(double *const pp1,double const*const p,  sConst const*const psc);
int stepDiff_real_cn(double *const pp1,double const*const p,  sConst const*const psc);
int addfields(double *const pp1,double const*const p,double const*const  rhs, double const dFactor, sFieldSize const*const sFs);
int cleanfields(double *const pp1,double const*const p,double const*const  rhs, double const dFactor, sFieldSize const*const sFs);
void swapFields(double **f1,double **f2);
void copyandmultfields(double *const pp1,double const*const p, double const dFactor, sFieldSize const*const sFs);

void fourier_bc(double *const field,sFieldSize const*const sFs);
void real_bc(double *const field,sFieldSize const*const sFs);
void null_bc(double *const field,sFieldSize const*const sFs);

int getParams(sConst *const psc);
void freeParams(sConst *psc);
int writeProfil(const char *const cFileName, const double *const u, sFieldSize const*const sFs,H2D_IO_FORMAT ioformat);
void calcMatInit(double *const pMatInit, sFieldSize const*const sFs, sFieldDiscret const*const sFd);




int initInvLapStructXF(sInvLap * sIL, sFieldSize const *const sFs, double const dDx, double const dKy, double const dFactor);
int initInvLapStructX(sInvLap * sIL, sFieldSize const *const sFs, double const dDx, double const dFactor);
int initInvLapStructY(sInvLap * sIL, sFieldSize const *const sFs, double const dDy, double const dFactor);
int invMat(double const*const pfP, double *const pfInvP, sFieldSize const *const sFs, sInvLap * sIL);
int  freeInvLapStruct(sInvLap * sIL);
int  mallocInvLapStruct(sInvLap *sLap, sFieldSize const*const sFs);
void sor(double *const u,double *const RHS,sSOR const*const sMat,sFieldSize const*const sFs);
void initSOR(sSOR *const SorMat,sConst const*const psc, double const dFactor);
int  mallocSORStruct(sSOR *sLap, sFieldSize const*const sFs);
int  freeSORStruct(sSOR * sIL);

#endif