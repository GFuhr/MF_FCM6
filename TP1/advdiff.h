#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>


/* structure contenant l'ensemble des parametres, tableaux intermediaires importants */
typedef struct
{
    double dx;
    double dt;
    double V;
    double C;
    double A;
    double x0;
    double sigma;
    unsigned int Nx;
    unsigned int Npas;
    unsigned int Nout;
    double *Udiff;
    double *Uadv;
    double *Utmp;
    double *du1;
    double *du2;
    double *du3;
    double *du4;
    double *pdA;
    double *pdB;
    double *pdC;
    double *pdGam;
    double *pdBet;
}sParam, *psParam;

/*pointeurs de fonctions */
typedef int (*pfFuncOp)     (const double *const , const double *const ,  double *const , const sParam *const ,const double );
typedef int (*pfFuncTime)   (const double *const , const double *const ,  double *const , const sParam *const , pfFuncOp );
typedef double (*pfProfInit)(const double        , const double        , const double   , const double);

/*routine de calcul des differents operateurs */
int advec (const double *const x, const double *const un,  double *const unp1,const sParam *const psp, const double dFac);
int diffus(const double *const x, const double *const un,  double *const unp1,const sParam *const psp, const double dFac);
int advdif(const double *const x, const double *const un,  double *const unp1,const sParam *const psp, const double dFac);

/*routines de creation des champs, calcul des conditions de bords...*/
int boundary(double *const u, const unsigned int N);
int initProf( double  *const x,  double *const u0,const sParam *const psp,  pfProfInit const pfpi);
double sinus(const double, const double, const double, const double);
double gaussienne(const double, const double, const double, const double);
double porte(const double, const double, const double, const double);
double heavyside(const double, const double, const double, const double);



int initParam(psParam const psp);
int freeParam(psParam const psp);
int printParam(psParam const psp);


int swapFields(double **f1,double **f2);

int addField(const double *const f1, const double *const f2, double *const fres, const unsigned int N);
int addField2(const double *const f1, const double *const f2, double *const fres, const unsigned int N, const double dFac);

/*schemas temporels */
int resol_RK4   (const double *const x, const double *const un,  double *const unp1,const sParam *const psp, pfFuncOp pfop);
int resol_Euler (const double *const x, const double *const un,  double *const unp1,const sParam *const psp, pfFuncOp pfop);
int resol_EulerI(const double *const x, const double *const un,  double *const unp1,const sParam *const psp, pfFuncOp pfop);


/*saisies utilisateurs */
int getInt(void);
double getDouble(void);

/*routine d'ecriture des profil/gestion des noms de fichiers */
int isFileExist(const char *const cFileName);
int GenerateFileName(const char *const cFilePrefix, char *const cGenerateName);
int writeProfil(const char *const cFileName, const double *const x, const double *const u, const unsigned int N);

/*routines associees aux methodes implicites */
int invMat1D(const double *const pSrc, double *const pDest, const unsigned int iSizeX, const double *const a, const double *const pGam, const double *const pBet);
int initInvMat1D(const unsigned int iSizeX, const double *const a, const double *const b, const double *const c, double *const pGam, double *const pBet);
int CreateMatrixDiffus(  sParam *const psp);
int CreateMatrixAdvec(   sParam *const psp);
int CreateMatrixAdvDiff( sParam *const psp);
