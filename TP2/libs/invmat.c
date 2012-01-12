/**
*\file Invmat.c
*\author G. Fuhr
*\version 2.0
*\date 11 Fevrier 2008
*
* routines pour les inversions de matrice, attention, routines ne marchant pas si le domaine radial est decoupe 
* gestion des erreurs a reprendre pour la v2.1
*/

#include "../include/H2D_types.h"
#include "../include/libraries.h"
#include "../include/common.h"
#include "../include/H2D_Func.h"

static void cyclic(double const*const res, double *const pFieldInv,sFieldSize const*const sFs,sInvLap const*const sIL);
static int invMat1D(pcnstField1D_cnst pfP, pcnstField1D pfInvP, pcnstField1D_cnst pMatA, pcnstField1D_cnst pGam, double const*const pBet, long const iSizeX);
static int invMat1DY(pcnstField1D_cnst pfP, pcnstField1D pfInvP, pcnstField1D_cnst pMatA, pcnstField1D_cnst pGam, double const*const pBet,long const iPosX, long const iSizeY, long const iSizeX);
static void generateInvMatData(double *const pMatA, double *const pMatB, double *const pMatC, double *const pGam, double *const pBet, long const sDiagSize);

/**
*\fn RBM_codes  mallocInvLapStruct(sLap, sFieldSize const*const sFs)
*\brief 
*\param  : 
*\param  : 
*\param  : 
*\return : RBM_OK
*/
int  mallocInvLapStruct(sInvLap *sLap, sFieldSize const*const sFs)
{
    static const char FCNAME[] = "mallocInvLapStruct";
    int rbm_er = 0;

    sLap->gam = allocArray1D(sFs->iSize2*sFs->iSize1,sizeof(*(sLap->gam)));
    if (sLap->gam == NULL)
    {            rbm_er=1;        }

    sLap->bet = allocArray1D(sFs->iSize2*sFs->iSize1,sizeof(*(sLap->bet)));
    if (sLap->bet == NULL)
    {            rbm_er=1;        }

    sLap->a = allocArray1D(sFs->iSize1,sizeof(*(sLap->a)));
    if (sLap->a == NULL)
    {            rbm_er=1;        }

    sLap->u = allocArray1D(sFs->iSize2,sizeof(*(sLap->u)));
    if (sLap->u == NULL)
    {            rbm_er=1;        }

    sLap->z = allocArray1D(sFs->iSize2,sizeof(*(sLap->z)));
    if (sLap->z == NULL)
    {            rbm_er=1;        }


    return rbm_er;
}





/**
*\fn RBM_codes  freeInvLapStruct(sInvLap * sIL)
*\brief 
*\param  : 
*\param  : 
*\param  : 
*\return : RBM_OK
*/
int  freeInvLapStruct(sInvLap * sIL)
{
    static const char FCNAME[] = "freeInvLapStruct";
    int rbm_er = 0;

    freeArray1D(sIL->z),sIL->z=NULL;
    freeArray1D(sIL->u),sIL->u=NULL;
    freeArray1D(sIL->a),sIL->a=NULL;
    freeArray1D(sIL->bet),sIL->bet=NULL;
    freeArray1D(sIL->gam),sIL->gam=NULL;

    return rbm_er;
}


static void generateTriDiagData(double *const pMatA, double *const pMatB, double *const pMatC, double const dX,double const dKy2, double const dAddDiag,double const dFactor, H2D_BC const dBC, long const sDiagSize)
{
    double const dInvDr2 = 1./(dX*dX);
    long j=0;

    for(j=0;j<(sDiagSize);j++)
    {
        pMatA[j] = dFactor*(dInvDr2);
        pMatC[j] = dFactor*(dInvDr2);
        pMatB[j] = dFactor*(-2.*dInvDr2-dKy2)+dAddDiag;

    }

    if (dBC==_DIR)
    {
    pMatC[0]=0.;
    pMatC[1]=0.;
    pMatA[sDiagSize-2]=0.;
    pMatA[sDiagSize-1]=0.;
    }
    else if (dBC==_PER)
        {
            pMatC[0]=0.;
            pMatA[sDiagSize-1]=0.;
        }
}

static void generateInvMatData(double *const pMatA, double *const pMatB, double *const pMatC, double *const pGam, double *const pBet, long const sDiagSize)
{
    long k=0;

    pBet[1]=pMatB[1];

#if defined(__ICC) || defined(__INTEL_COMPILER)
#pragma novector
#endif
    for(k=2;k<sDiagSize;k++)
    {
        pGam[k] = pMatC[k-1]/pBet[k-1];
        pBet[k]  = pMatB[k]-pMatA[k]*pGam[k];
    }

    for(k=1;k<(sDiagSize);k++)
        pBet[k]=1./pBet[k];
}

/**
*\fn RBM_codes initInvLapStruct(sInvLap * sIL, sFieldSize const *const sFs, _VARTYPE_ const dDx, _VARTYPE_ const dKy,sGeomOp const*const sGOp)
*\brief iniatilise les matrices necessaires a la routine tridiag du NR (vec : a,gam,bet)
* attention routine utilisable uniquement dans le cas monoproc...
*\param psc : structure contenant les donnees
*\return : RBM_OK
*/
int initInvLapStructXF(sInvLap * sIL, sFieldSize const *const sFs, double const dDx, double const dKy, double const dFactor)
{
    static const char FCNAME[] = "initMatLap";
    int rbm_er = 0;
    long j;
    const long iXLen = sFs->iSize1;
    double *const c=allocArray1D(iXLen,sizeof(*c));
    double *const pmlMat=  allocArray1D(sFs->iSize2*sFs->iSize1,sizeof(*pmlMat));
    double * pMatA=sIL->a;
    pcnstField1D pGam=sIL->gam;
    pcnstField1D pBet=sIL->bet;

    sIL->dir=_XF;
    if ((c==NULL)||(pmlMat==NULL))
    {

        return 1;
    }

    for(j=0;j<sFs->iSize2;j++)
    {
        double mky2 = 0.;
        if (fabs(dKy)>EPS)
        {
            mky2=(double)j*(double)j*dKy*dKy;
        }

        generateTriDiagData(pMatA,pmlMat+sFs->iSize1*j,c,dDx,mky2,1,dFactor,_DIR,sFs->iSize1);
    }

    for(j=0;j<sFs->iSize2;j++)
    {

        generateInvMatData(pMatA,pmlMat+sFs->iSize1*j,c,pGam+sFs->iSize1*j,pBet+sFs->iSize1*j,sFs->iSize1);
    }

    freeArray1D(pmlMat);
    freeArray1D(c);

    return rbm_er;
}

int initInvLapStructX(sInvLap * sIL, sFieldSize const *const sFs, double const dDx, double const dFactor)
{
    static const char FCNAME[] = "initMatLap";
    int rbm_er = 0;
    const long iXLen = sFs->iSize1;
    double *const c=allocArray1D(iXLen,sizeof(*c));
    double *const pmlMat=  allocArray1D(sFs->iSize1,sizeof(*pmlMat));


    sIL->dir=_X;
    if ((c==NULL)||(pmlMat==NULL))
    {
        return 1;
    }

    generateTriDiagData(sIL->a,pmlMat,c,dDx,0,1,dFactor,_DIR,sFs->iSize1);
    generateInvMatData(sIL->a,pmlMat,c,sIL->gam,sIL->bet,sFs->iSize1);

    freeArray1D(pmlMat);
    freeArray1D(c);

    return rbm_er;
}


/**
*\fn RBM_codes initInvLapStruct(sInvLap * sIL, sFieldSize const *const sFs, _VARTYPE_ const dDx, _VARTYPE_ const dKy,sGeomOp const*const sGOp)
*\brief iniatilise les matrices necessaires a la routine tridiag du NR (vec : a,gam,bet)
* attention routine utilisable uniquement dans le cas monoproc...
*\param psc : structure contenant les donnees
*\return : RBM_OK
*/
int initInvLapStructY(sInvLap * sIL, sFieldSize const *const sFs, double const dDy, double const dFactor)
{
    static const char FCNAME[] = "initInvLapStructY";

    long i;
    const long iYLen = sFs->iSize2;
    double const dInvDr2 = 1./(dDy*dDy);
    double gamma;
    double const alpha = dFactor/dInvDr2;
    double const beta = dFactor/dInvDr2;
    double *const c=allocArray1D(iYLen,sizeof(*c));
    double *const u=allocArray1D(iYLen,sizeof(*u));
    double * pmlMat=  allocArray1D(sFs->iSize2,sizeof(*pmlMat));


    sIL->dir=_Y;
    if ((c==NULL)||(pmlMat==NULL))
    {
        return 1;
    }

    generateTriDiagData(sIL->a,pmlMat,c,dDy,0,1,dFactor,_PER,iYLen);

    gamma = -pmlMat[1];
    pmlMat[1]=pmlMat[1]-gamma;                         /*Set up the diagonal of the modified tridi*/
    pmlMat[iYLen-2]=pmlMat[iYLen-2]-alpha*beta/gamma;  /*agonal system.*/
    sIL->beta = beta;
    sIL->gamma = gamma;

    generateInvMatData(sIL->a,pmlMat,c,sIL->gam,sIL->bet,iYLen);


    u[1]=gamma;                             /*Set up the vector u.*/
    u[iYLen-2]=alpha;
    for (i=2;i<iYLen-2;i++) u[i]=0.0;


    invMat1D(u,sIL->z,sIL->a,sIL->gam,sIL->bet,iYLen);
    freeArray1D(pmlMat);
    freeArray1D(c);
    freeArray1D(u);

    return 0;
}




/**
*\fn RBM_codes RBM_codes invMat3Dc(pcnstField3Dc_cnst pfP, pcnstField3Dc pfInvP, sFieldSize const *const sFs, sInvLap * sIL)
*\brief calcule le laplacien inverse d'un champ
* attention routine utilisable uniquement dans le cas monoproc... 
*\param pfP : champ qu'on veut 'inverser'
*\param pfInvP : champ qui va contenir le laplacien inverse
*\param sFs : taille du champ
*\param sIL : 
*\return : rien
*/
int invMat(double const*const pfP, double *const pfInvP, sFieldSize const *const sFs, sInvLap * sIL)
{
    static const char FCNAME[] = "invMat";
    int rbm_er = 0;
    long j,k;
    long const iLenX=sFs->iSize1;
    long const iLenY=sFs->iSize2;


    if ((sIL->dir==_XF))
    {
        pcnstField1D_cnst a=sIL->a;
#ifdef RBM_USE_OMP
#if (defined(__GNUC__) && !defined(__ICC))
#pragma omp parallel for private(i,j,k) 
#else
#pragma omp parallel for private(i,j,k) 
#endif
#endif

        for(j=0;j<iLenY;j++)
        {
            pcnstField1D_cnst pscmPr=&(pfP[j*iLenX]);
            pcnstField1D_cnst pscmPi=&(pfP[j*iLenX+iLenX*iLenY]);
            pcnstField1D_cnst pTmp=&(sIL->bet[j*iLenX]);
            pcnstField1D_cnst pTmp1=&(sIL->gam[j*iLenX]);

            pcnstField1D pscmInvPr=&(pfInvP[j*iLenX]);
            pcnstField1D pscmInvPi=&(pfInvP[j*iLenX+iLenX*iLenY]);


            pscmInvPr[1]=pscmPr[1]*pTmp[1];
            pscmInvPi[1]=pscmPi[1]*pTmp[1];

#if defined(__ICC) || defined(__INTEL_COMPILER) 
#pragma novector
#endif
            for(k=2;k<iLenX;k++)
            {
                pscmInvPr[k]=(pscmPr[k]-a[k]*pscmInvPr[k-1])*pTmp[k];
                pscmInvPi[k]=(pscmPi[k]-a[k]*pscmInvPi[k-1])*pTmp[k];
            }

#if defined(__ICC) || defined(__INTEL_COMPILER) 
#pragma novector
#endif
            for(k=(iLenX-2);k>=1;k--)
            {
                pscmInvPr[k]-=pTmp1[k+1]*pscmInvPr[k+1];
                pscmInvPi[k]-=pTmp1[k+1]*pscmInvPi[k+1];
            }
        }
    }


    if ((sIL->dir==_X))
    {

#ifdef RBM_USE_OMP
#if (defined(__GNUC__) && !defined(__ICC))
#pragma omp parallel for private(i,j,k) 
#else
#pragma omp parallel for private(i,j,k) 
#endif
#endif
        for(j=1;j<iLenY-1;j++)
        {
            invMat1D(pfP+j*iLenX,pfInvP+j*iLenX,sIL->a,sIL->gam,sIL->bet,iLenX);
        }
    }


    if ((sIL->dir==_Y))
    {
        cyclic(pfP,pfInvP,sFs,sIL);
    }


    return rbm_er;
}


/**
*\fn RBM_codes RBM_codes invMat1D(pcnstField1D_cnst pfP, pcnstField1D pfInvP, pcnstField1D_cnst pMatA, pcnstField1D_cnst pGam, pcnstField1D_cnst pBet, size_t const iSizeX)
*\brief calcule le laplacien inverse d'un vecteur reel 1D
* attention routine utilisable uniquement dans le cas monoproc... 
*\param pfP : champ qu'on veut 'inverser'
*\param pfInvP : champ qui va contenir le laplacien inverse
*\param pMatA : matrice 'a' de l'algo du NR
*\param pGam : matrice 'gam' du NR 
*\param pBet : matrice 'bet' du NR 
*\param iSizeX : taille du vecteur
*\return : rien
*/
static int invMat1D(pcnstField1D_cnst pfP, pcnstField1D pfInvP, pcnstField1D_cnst pMatA, pcnstField1D_cnst pGam, pcnstField1D_cnst pBet, long const iSizeX)
{
    static const char FCNAME[] = "invMat1D";
    int rbm_er = 0;
    size_t k;


    pfInvP[1]=pfP[1]*pBet[1];


#if defined(__ICC) || defined(__INTEL_COMPILER) 
#pragma novector
#endif
    for(k=2;k<iSizeX-1;k++)
    {
        pfInvP[k]=(pfP[k]-pMatA[k]*pfInvP[k-1])*pBet[k];
    }

#if defined(__ICC) || defined(__INTEL_COMPILER) 
#pragma novector
#endif
    for(k=(iSizeX-3);k>=1;k--)
    {
        pfInvP[k]-=pGam[k+1]*pfInvP[k+1];
    }

    return rbm_er;
}

/**
*\fn RBM_codes RBM_codes invMat1D(pcnstField1D_cnst pfP, pcnstField1D pfInvP, pcnstField1D_cnst pMatA, pcnstField1D_cnst pGam, pcnstField1D_cnst pBet, size_t const iSizeX)
*\brief calcule le laplacien inverse d'un vecteur reel 1D
* attention routine utilisable uniquement dans le cas monoproc... 
*\param pfP : champ qu'on veut 'inverser'
*\param pfInvP : champ qui va contenir le laplacien inverse
*\param pMatA : matrice 'a' de l'algo du NR
*\param pGam : matrice 'gam' du NR 
*\param pBet : matrice 'bet' du NR 
*\param iSizeX : taille du vecteur
*\return : rien
*/
static int invMat1DY(pcnstField1D_cnst pfP, pcnstField1D pfInvP, pcnstField1D_cnst pMatA, pcnstField1D_cnst pGam, pcnstField1D_cnst pBet,long const iPosX, long const iSizeY, long const iSizeX)
{
    static const char FCNAME[] = "invMat1D";
    int rbm_er = 0;
    long k;

    pfInvP[iPosX+iSizeX]=pfP[iPosX+iSizeX]*pBet[1];

#if defined(__ICC) || defined(__INTEL_COMPILER) 
#pragma novector
#endif
    for(k=2;k<iSizeY-1;k++)
    {
        pfInvP[iPosX+k*iSizeX]=(pfP[iPosX+k*iSizeX]-pMatA[k]*pfInvP[iPosX+iSizeX*(k-1)])*pBet[k];
    }

#if defined(__ICC) || defined(__INTEL_COMPILER) 
#pragma novector
#endif
    for(k=(iSizeY-3);k>=1;k--)
    {
        pfInvP[iPosX+k*iSizeX]-=pGam[k+1]*pfInvP[iPosX+(k+1)*iSizeX];
    }

    return rbm_er;
}

static void cyclic(double const*const pField, double *const pFieldInv,sFieldSize const*const sFs,sInvLap const*const sIL)
{
    long i,n=sFs->iSize2;
    double fact;
    double const ratio = sIL->beta/sIL->gamma;
    double const denom = 1./((1.0+sIL->z[1]+ratio*sIL->z[n-2]));

    long x=0;


    /*     gamma = -b[1];                          *//*Avoid subtraction error in forming bb[0].*/
    /*     bb[1]=b[1]-gamma;                       *//*Set up the diagonal of the modified tridi*/
    /*     bb[n-2]=b[n-2]-alpha*beta/gamma;        *//*agonal system.*/
    /*         for (i=2;i<n-2;i++) bb[i]=b[i];*/
    /*tridag(a,bb,c,res,pFieldInv);*/                     /*Solve A * x = r.*/
    for(x=1;x<sFs->iSize1-1;x++)
    {
        invMat1DY(pField,pFieldInv,sIL->a,sIL->gam,sIL->bet,x,sFs->iSize2,sFs->iSize1);
        fact=(pFieldInv[x+sFs->iSize1]+ratio*pFieldInv[x+sFs->iSize1*(n-2)])*denom;
        for (i=1;i<n-1;i++) pFieldInv[x+sFs->iSize1*i] -= fact*sIL->z[i];
    }

}
