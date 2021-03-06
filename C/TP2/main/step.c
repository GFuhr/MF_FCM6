#include "../include/H2D_types.h"
#include "../include/H2D_Func.h"



/*calcul de Sm[y][x]+D2/Dx2(p)-m2 p */
int stepDiff_fourier(double *const pp1,double const*const p,  sConst const*const psc)
{
    long x=0;
    long y=0;
    double const dEtaFac=psc->dEta/(psc->sSteps->dDx1*psc->sSteps->dDx1);

    for(y=0;y<psc->sSize->iSize2;y++)
    {
        double const dEtaKy = psc->dEta*psc->pdMKy[y]*psc->pdMKy[y];
        for(x=1;x<psc->sSize->iSize1-1;x++)
        {
            long const posr = x+y*psc->sSize->iSize1;
            long const posi = x+y*psc->sSize->iSize1+psc->sSize->iSize1*psc->sSize->iSize2;
            pp1[posr]=psc->pdSource[posr]+dEtaFac*(p[posr+1]+p[posr-1]-2.*p[posr])-dEtaKy*p[posr];
            pp1[posi]=psc->pdSource[posi]+dEtaFac*(p[posi+1]+p[posi-1]-2.*p[posi])-dEtaKy*p[posi];
        }
    }

    return 0;
}


/*calcul de S[y][x]+D2/Dx2(p[y][x])+D2/Dy2(p[y][x]) */
int stepDiff_real(double *const pp1,double const*const p,  sConst const*const psc)
{
    long x=0;
    long y=0;
    double const dEtaFacDx=psc->dEta/(psc->sSteps->dDx1*psc->sSteps->dDx1);
    double const dEtaFacDy=psc->dEta/(psc->sSteps->dDx2*psc->sSteps->dDx2);

    for(y=1;y<psc->sSize->iSize2-1;y++)
    {

        for(x=1;x<psc->sSize->iSize1-1;x++)
        {
            long const posxy   = x+y*psc->sSize->iSize1;
            long const posxyp1= x+(y+1)*psc->sSize->iSize1;
            long const posxym1= x+(y-1)*psc->sSize->iSize1;
            pp1[posxy]=psc->pdSource[posxy]+dEtaFacDx*(p[posxy+1]+p[posxy-1]-2.*p[posxy])+dEtaFacDy*(p[posxyp1]+p[posxym1]-2.*p[posxy]);
        }
    }

    return 0;
}


/*calcul de S[y][x]+D2/Dx2(p[y][x])+D2/Dy2(p[y][x]) */
static int stepDiff_real_x(double *const pp1,double const*const p,  sConst const*const psc)
{
    long x=0;
    long y=0;
    double const dEtaFacDx=psc->dEta/(psc->sSteps->dDx1*psc->sSteps->dDx1);
    double const dEtaFacDy=psc->dEta/(psc->sSteps->dDx2*psc->sSteps->dDx2);

    for(y=1;y<psc->sSize->iSize2-1;y++)
    {

        for(x=1;x<psc->sSize->iSize1-1;x++)
        {
            long const posxy   = x+y*psc->sSize->iSize1;
            long const posxyp1= x+1+y*psc->sSize->iSize1;
            long const posxym1= x-1+y*psc->sSize->iSize1;
            pp1[posxy]=psc->pdSource[posxy]+dEtaFacDx*(p[posxy+1]+p[posxy-1]-2.*p[posxy]);
        }
    }

    return 0;
}



/*calcul de S[y][x]+D2/Dx2(p[y][x])+D2/Dy2(p[y][x]) */
static int stepDiff_real_y(double *const pp1,double const*const p,  sConst const*const psc)
{
    long x=0;
    long y=0;
    double const dEtaFacDx=psc->dEta/(psc->sSteps->dDx1*psc->sSteps->dDx1);
    double const dEtaFacDy=psc->dEta/(psc->sSteps->dDx2*psc->sSteps->dDx2);

    for(y=1;y<psc->sSize->iSize2-1;y++)
    {

        for(x=1;x<psc->sSize->iSize1-1;x++)
        {
            long const posxy   = x+y*psc->sSize->iSize1;
            long const posxyp1= x+(y+1)*psc->sSize->iSize1;
            long const posxym1= x+(y-1)*psc->sSize->iSize1;
            pp1[posxy]=psc->pdSource[posxy]+dEtaFacDy*(p[posxyp1]+p[posxym1]-2.*p[posxy]);
        }
    }

    return 0;
}


int stepDiff_real_cn(double *const pp1,double const*const p,  sConst const*const psc)
{
    if (psc->eDir==_X)
    {
        stepDiff_real_x(pp1,p,psc);
    }
    else if (psc->eDir==_Y)
    {
        stepDiff_real_y(pp1,p,psc);
    }
    else if (psc->eDir==_XF)
    {
        stepDiff_fourier(pp1,p,psc);
    }
    return 0;
}

/*calcul de pp1[y][x]=p[y][x]+ dFactor* rhs[y][x] */
int addfields(double *const pp1,double const*const p,double const*const  rhs, double const dFactor, sFieldSize const*const sFs)
{
    long y=0;
    long const iLen  = sFs->iSize1*sFs->iSize2*sFs->iSizeC;

    for(y=0;y<iLen;y+=2)
    {
            pp1[y]   = p[y]   + dFactor*rhs[y];
            pp1[y+1] = p[y+1] + dFactor*rhs[y+1];

    }

    return 0;
}


/*calcul de S[y][x]+D2/Dx2(p[y][x])+D2/Dy2(p[y][x]) */
void copyandmultfields(double *const pp1,double const*const p, double const dFactor, sFieldSize const*const sFs)
{
    long y=0;
    long const iLen  = sFs->iSize1*sFs->iSize2*sFs->iSizeC;

    for(y=0;y<iLen;y++)
    {
        pp1[y]=p[y]*dFactor;
    }


}

/* pp1[y][x] = 0 */
int cleanfields(double *const pp1,double const*const p,double const*const  rhs, double const dFactor, sFieldSize const*const sFs)
{
    long y=0;
    long const iLen  = sFs->iSize1*sFs->iSize2*sFs->iSizeC;

    for(y=0;y<iLen;y++)
    {
        pp1[y]=0.;
    }

    return 0;
}


/* arrays associated to f1 and f2 are switched : f1->f2 and f2->f1*/
void swapFields(double **f1,double **f2)
{
    double *ptmp;
    ptmp=*f1;
    *f1=*f2;
    *f2=ptmp;

}
