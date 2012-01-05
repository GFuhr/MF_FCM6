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


static void cyclic(double const*const res, double *const pFieldInv,sFieldSize const*const sFs,sInvLap const*const sIL);
static int invMat1D(pcnstField1D_cnst pfP, pcnstField1D pfInvP, pcnstField1D_cnst pMatA, pcnstField1D_cnst pGam, pcnstField1D_cnst pBet, long const iSizeX);
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





/**
*\fn RBM_codes initInvLapStruct(sInvLap * sIL, sFieldSize const *const sFs, _VARTYPE_ const dDx, _VARTYPE_ const dKy,sGeomOp const*const sGOp)
*\brief iniatilise les matrices necessaires a la routine tridiag du NR (vec : a,gam,bet)
* attention routine utilisable uniquement dans le cas monoproc...
*\param psc : structure contenant les donnees
*\return : RBM_OK
*/
int initInvLapStruct(sInvLap * sIL, sFieldSize const *const sFs, double const dDx, double const dKy)
{
    static const char FCNAME[] = "initMatLap";
    int rbm_er = 0;
    size_t i,j,k;
    const size_t iXLen = sFs->iSize1;
    double const dInvDr2 = 1./(dDx*dDx);

    double *const c=allocArray1D(iXLen,sizeof(*c));

    pField2D pmlMat=  allocArray2D(sFs->iSize2,sFs->iSize1,sizeof(**pmlMat));

    double * pMatA=sIL->a;
    pcnstField1D pGam=sIL->gam;
    pcnstField1D pBet=sIL->bet;


    if ((c==NULL)||(pmlMat==NULL))
    {

        return 1;
    }


    for(j=1;j<(iXLen-1);j++)
    {
        pMatA[j] = dInvDr2;
        c[j]     = dInvDr2;
    }
    c[1]=0.;
    pMatA[iXLen-2]=0.;



    for(j=0;j<sFs->iSize2;j++)
    {
        pcnstField1D ptemp=pmlMat[j];
        double ptemp2 = -2.*dInvDr2;
        double mky2 = 0.;
        if (fabs(dKy)>EPS)
        {
            mky2=(double)j*(double)j*dKy*dKy;
        }


#ifdef RBM_USE_OMP
#pragma omp for
#endif
        for(k=1;k<(iXLen-1);k++)
            ptemp[k]=ptemp2-mky2;
    }

    for(j=0;j<sFs->iSize2;j++)
    {
        pcnstField1D ptmp       =   &(pBet[j*sFs->iSize1]);
        pcnstField1D_cnst ptmp2 =   pmlMat[j];
        pcnstField1D ptmp1      =   &(pGam[j*sFs->iSize1]);

        ptmp[1]=ptmp2[1];

#if defined(__ICC) || defined(__INTEL_COMPILER)
#pragma novector
#endif
        for(k=2;k<iXLen-1;k++)
        {
            ptmp1[k] = c[k-1]/ptmp[k-1];
            ptmp[k]  = ptmp2[k]-pMatA[k]*ptmp1[k];
        }
    }

    for(j=0;j<sFs->iSize2;j++)
    {
        pcnstField1D ptmp= &(pBet[j*sFs->iSize1]);
#ifdef RBM_USE_OMP
#pragma omp for
#endif
        for(k=1;k<(iXLen-1);k++)
            ptmp[k]=1./ptmp[k];
    }

    freeArray2D(pmlMat),pmlMat=NULL;
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
int initInvLapStructY(sInvLap * sIL, sFieldSize const *const sFs, double const dDy)
{
    static const char FCNAME[] = "initInvLapStructY";

    size_t i,j,k;
    const size_t iYLen = sFs->iSize2;
    double const dInvDr2 = 1./(dDy*dDy);
    double fact,gamma;
    double const alpha = dInvDr2;
    double const beta = dInvDr2;

    double *const c=allocArray1D(iYLen,sizeof(*c));
    double *const u=allocArray1D(iYLen,sizeof(*u));

    double * pmlMat=  allocArray1D(sFs->iSize2,sizeof(*pmlMat));

    double * pMatA=sIL->a;
    pcnstField1D pGam=sIL->gam;
    pcnstField1D pBet=sIL->bet;


    if ((c==NULL)||(pmlMat==NULL))
    {

        return 1;
    }


    for(j=1;j<(iYLen-1);j++)
    {
        pMatA[j] = dInvDr2;
        c[j]     = dInvDr2;
    }
    c[1]=0.;
    pMatA[iYLen-2]=0.;


    {
        pcnstField1D ptemp=pmlMat;
        double ptemp2 = -2.*dInvDr2;

#ifdef RBM_USE_OMP
#pragma omp for
#endif
        for(k=1;k<(iYLen-1);k++)
            ptemp[k]=ptemp2;
    }
    gamma = -pmlMat[1];
    pmlMat[1]=pmlMat[1]-gamma;                       /*Set up the diagonal of the modified tridi*/
    pmlMat[iYLen-2]=pmlMat[iYLen-2]-alpha*beta/gamma;        /*agonal system.*/
    sIL->beta = beta;
    sIL->gamma = gamma;

    {
        pcnstField1D ptmp       =   pBet;
        pcnstField1D_cnst ptmp2 =   pmlMat;
        pcnstField1D ptmp1      =   pGam;

        ptmp[1]=ptmp2[1];

#if defined(__ICC) || defined(__INTEL_COMPILER)
#pragma novector
#endif
        for(k=2;k<iYLen-1;k++)
        {
            ptmp1[k] = c[k-1]/ptmp[k-1];
            ptmp[k]  = ptmp2[k]-pMatA[k]*ptmp1[k];
        }
    }


    {
        pcnstField1D ptmp=pBet;
#ifdef RBM_USE_OMP
#pragma omp for
#endif
        for(k=1;k<(iYLen-1);k++)
            ptmp[k]=1./ptmp[k];
    }

    u[1]=gamma;                             /*Set up the vector u.*/
    u[iYLen-2]=alpha;
    for (i=2;i<iYLen-2;i++) u[i]=0.0;


    invMat1D(u,sIL->z,sIL->a,pGam,pBet,iYLen);
    freeArray1D(pmlMat),pmlMat=NULL;
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
    static const char FCNAME[] = "invMat3Dc";
    int rbm_er = 0;
    size_t i,j,k,x,y,iLenX,iLenY;
    pcnstField1D_cnst a=sIL->a;


    iLenX=sFs->iSize1;
    iLenY=sFs->iSize2;


    if ((sIL->dir==_XF)||(sIL->dir==_X))
    {

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
    for(k=2;k<iSizeX;k++)
    {
        pfInvP[k]=(pfP[k]-pMatA[k]*pfP[k-1])*pBet[k];
    }

#if defined(__ICC) || defined(__INTEL_COMPILER) 
#pragma novector
#endif
    for(k=(iSizeX-2);k>=1;k--)
    {
        pfInvP[k]-=pGam[k+1]*pfInvP[k+1];
    }

    return rbm_er;
}


static void cyclic(double const*const res, double *const pFieldInv,sFieldSize const*const sFs,sInvLap const*const sIL)
{
    long i,n=sFs->iSize2;
    double fact,gamma;


    /*     gamma = -b[1];                          *//*Avoid subtraction error in forming bb[0].*/
    /*     bb[1]=b[1]-gamma;                       *//*Set up the diagonal of the modified tridi*/
    /*     bb[n-2]=b[n-2]-alpha*beta/gamma;        *//*agonal system.*/
    /*         for (i=2;i<n-2;i++) bb[i]=b[i];*/
    /*tridag(a,bb,c,res,pFieldInv);*/                     /*Solve A * x = r.*/
    invMat1D(res,pFieldInv,sIL->a,sIL->gam,sIL->bet,sFs->iSize2);
    fact=(pFieldInv[1]+sIL->beta*pFieldInv[n-2]/sIL->gamma)/          /*Form v  x=.1Cv  z/.*/
        (1.0+sIL->z[1]+sIL->beta*sIL->z[n-2]/gamma);
    for (i=1;i<n-1;i++) pFieldInv[i] -= fact*sIL->z[i];


}