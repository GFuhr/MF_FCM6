#include "advdiff.h"



int main(int argc, char **argv)
{
    double *Udiff=NULL, *Uadv=NULL, *Un=NULL, *Unp1=NULL, *Xval=NULL, *Utmp=NULL;
    int choix=0;
    int choix1=0;
    char cBuffer[256]={0};
    unsigned int i=0,j=0;
    unsigned int k=0;
    psParam psp=NULL;
    FILE *file;
    pfFuncOp pfOperator=NULL;
    pfFuncTime pfTime=NULL;
    pfProfInit pfFuncU0=NULL;


    /* allocation de la structure */
    psp=malloc(sizeof(*psp));

    /* initialisation de la structure et des parametres */
    fprintf(stdout,"***Parametres***\n");
    initParam(psp);

    /*choix des termes et de la methode de resolution */
    fprintf(stdout,"***Choix des termes de l'equation***\n");
    fprintf(stdout,"1-Diffusion\n");
    fprintf(stdout,"2-Advection\n");
    fprintf(stdout,"3-Diffusion+Advection\n");
    choix1=getInt();


    switch(choix1)
    {
    case 1: pfOperator=diffus; break;
    case 2: pfOperator=advec;  break;
    case 3: pfOperator=advdif; break;
    default: break;
    }
    fprintf(stdout,"***Choix de la methode de resolution***\n");
    fprintf(stdout,"1-Euler explicite\n");
    fprintf(stdout,"2-Euler implicite\n");
    fprintf(stdout,"3-Runge-Kutta 4\n");
    choix=getInt();

    switch(choix)
    {
    case 1: pfTime=resol_Euler; break;
    case 2: pfTime=resol_EulerI;  break;
    case 3: pfTime=resol_RK4; break;
    default: break;
    }


    fprintf(stdout,"***Choix du profil initial***\n");
    fprintf(stdout,"1-Sinus\n");
    fprintf(stdout,"2-Gaussienne\n");
    fprintf(stdout,"3-Fonction porte\n");
    choix=getInt();

    switch(choix)
    {
    case 1: pfFuncU0=sinus; psp->sigma=psp->sigma/((psp->dx)*(psp->Nx-3));      break;
    case 2: pfFuncU0=gaussienne;  break;
    case 3: pfFuncU0=porte;       break;
    default: break;
    }


    /* allocation memoire pour les tableaux */
    Udiff = malloc(sizeof(*Udiff)*psp->Nx);
    Uadv  = malloc(sizeof(*Uadv)*psp->Nx);
    Un    = malloc(sizeof(*Un)*psp->Nx);
    Unp1  = malloc(sizeof(*Unp1)*psp->Nx);
    Utmp  = malloc(sizeof(*Utmp)*psp->Nx);
    Xval  = malloc(sizeof(*Xval)*psp->Nx);

    /* initialisation des valeurs de x et u0 */
    initProf(Xval,Un,psp,pfFuncU0);

    /* choix du shema implicite, initialisation matricielle en consequence */
    switch(choix1)
    {
    case 1: CreateMatrixDiffus(psp); break;
    case 2: CreateMatrixAdvec(psp);  break;
    case 3: CreateMatrixAdvDiff(psp); break;
    default: break;
    }
    if (choix1>0)
        initInvMat1D(psp->Nx, psp->pdA, psp->pdB, psp->pdC, psp->pdGam, psp->pdBet);


    /* ecriture des profils initiaux */
    GenerateFileName("out_",cBuffer);
    file=fopen(cBuffer,"w+");
    fclose(file);
    writeProfil(cBuffer,Xval,Un,psp->Nx);

    /* boucle en temps */
    for(i=0;i<psp->Npas;i+=psp->Nout)
    {
        for(j=0;j<psp->Nout;j++)
        {
            /* avancement en temps */
            pfTime(Xval,Un,Unp1,psp,pfOperator);

            /*pour le pas d'apres Unp1 va devenir le nouveau Un */
            swapFields(&Un,&Unp1);
        }
        /*ecriture des profils tout les Nout iterations */
        GenerateFileName("out_",cBuffer);
        writeProfil(cBuffer,Xval,Unp1,psp->Nx);
    }

    /* liberation de la memoire */
    freeParam(psp);
    free(psp),psp=NULL;
    free(Xval)  ,Xval=NULL;
    free(Unp1)  ,Unp1=NULL;
    free(Un)    ,Un=NULL;
    free(Uadv)  ,Uadv=NULL;
    free(Udiff) ,Udiff=NULL;
    free(Utmp)  ,Utmp=NULL;

    return 0;
}


/* utilisation securisee de scanf pour saisir un nombre entier */
int getInt(void)
{
    int nombre=0;
    int isOk=0;
    int retour=0;

    while (!isOk){

        retour = scanf("%d%*[^\n]", &nombre);

        if ( !retour ){
            /* erreur de saisie, on vide le flux */
            int c;
            while ( ((c = getchar()) != '\n') && c != EOF);


        }
        else {
            /* reussite de la saisie */
            getchar(); /* on enleve le '\n' restant */


            isOk = 1;  /* sort de la boucle */
        }

    }

    return nombre;
}

/* utilisation securisee de scanf pour saisir un nombre decimal */
double getDouble(void)
{
    double nombre=0;
    int isOk=0;
    int retour=0;

    while (!isOk){

        retour = scanf("%lf%*[^\n]", &nombre);

        if ( !retour ){
            /* erreur de saisie, on vide le flux */
            int c;
            while ( ((c = getchar()) != '\n') && c != EOF);


        }
        else {
            /* reussite de la saisie */
            getchar(); /* on enleve le '\n' restant */

            isOk = 1;  /* sort de la boucle */
        }

    }

    return nombre;
}


/* demande de saisie des parametres utilisateurs */
int initParam(psParam const psp)
{
    fprintf(stdout,"Nx=?");
    psp->Nx=getInt();
    fprintf(stdout,"Npas=?");
    psp->Npas=getInt();
    fprintf(stdout,"Nout=?");
    psp->Nout=getInt();
    fprintf(stdout,"C=?");
    psp->C=getDouble();
    fprintf(stdout,"V=?");
    psp->V=getDouble();
    fprintf(stdout,"A=?");
    psp->A=getDouble();
    fprintf(stdout,"x0=?");
    psp->x0=getDouble();
    fprintf(stdout,"sigma=?");
    psp->sigma=getDouble();
    fprintf(stdout,"Dx=?");
    psp->dx=getDouble();
    fprintf(stdout,"Dt=?");
    psp->dt=getDouble();

    fprintf(stdout,"dt %lf\n",psp->dt);
    psp->du1   = malloc( sizeof(*psp->du1)  *psp->Nx);
    psp->du2   = malloc( sizeof(*psp->du2)  *psp->Nx);
    psp->du3   = malloc( sizeof(*psp->du3)  *psp->Nx);
    psp->du4   = malloc( sizeof(*psp->du4)  *psp->Nx);
    psp->Utmp  = malloc( sizeof(*psp->Utmp) *psp->Nx);
    psp->Udiff = malloc( sizeof(*psp->Udiff)*psp->Nx);
    psp->Uadv  = malloc( sizeof(*psp->Uadv) *psp->Nx);
    psp->pdA   = malloc( sizeof(*psp->pdA)  *psp->Nx);
    psp->pdB   = malloc( sizeof(*psp->pdB)  *psp->Nx);
    psp->pdC   = malloc( sizeof(*psp->pdC)  *psp->Nx);
    psp->pdGam = malloc( sizeof(*psp->pdGam)*psp->Nx);
    psp->pdBet = malloc( sizeof(*psp->pdBet)*psp->Nx);
    return 0;
}

/*liberation de la memoire associee a une structure sParam */
int freeParam(sParam *const psp)
{
    free(psp->du1);
    free(psp->du2);
    free(psp->du3);
    free(psp->du4);
    free(psp->Utmp);
    free(psp->Udiff);
    free(psp->Uadv);
    free(psp->pdA);
    free(psp->pdB);
    free(psp->pdC);
    free(psp->pdGam);
    free(psp->pdBet);
    return 0;
}

/*condition de bords avec une precision dx^2*/
int boundary(double *const u, const unsigned int N)
{
    u[0]=-u[2];
    u[N-1]=-u[N-3];
    return 0;

}

/* calcul d'un terme d'advection */
int advec(const double *const x, const double *const un,  double *const unp1,const sParam *const psp, const double dFac)
{
    unsigned int i;
    double tmp;

    tmp=dFac*(-1.*psp->V)/(2.*psp->dx);
    for(i=1;i<psp->Nx-1;i++)
        unp1[i]=tmp*(un[i]-un[i-1]);


    /*calcul des B.C.*/
    boundary(unp1,psp->Nx);

    return 0;
}

/* calcul d'un terme de diffusion */
int diffus(const double *const x, const double *const un,  double *const unp1,const sParam *const  psp, const double dFac)
{
    unsigned int i;
    double tmp;

    tmp=dFac*psp->C/(psp->dx*psp->dx);

    for(i=1;i<psp->Nx-1;i++)
        unp1[i]=tmp*(un[i+1]+un[i-1]-2*un[i]);

    /*calcul des B.C.*/
    boundary(unp1,psp->Nx);

    return 0;
}

/* calcul d'un terme d'advection-diffusion */
int advdif(const double *const x, const double *const un,  double *const unp1,const sParam *const  psp, const double dFac)
{
    double *Udiff=NULL,*Uadv=NULL;

    Udiff=psp->Udiff;
    Uadv=psp->Uadv;


    /* calcul de la diffusion */
    diffus(x,un,Udiff,psp,dFac);
    /* calcul de l'advection */
    advec(x,un,Uadv,psp,dFac);
    /*addition des 2 termes */
    addField(Udiff,Uadv,unp1,psp->Nx);

    return 0;
}

/*creation d'un profil à partir d'une fonction donnee */
int initProf( double  *const x,  double *const u0,const sParam *const psp, const pfProfInit  pfpi)
{
    unsigned int i;


    for(i=0;i<psp->Nx;i++)
    {
        x[i]=i*psp->dx;
        u0[i]=pfpi(psp->x0,x[i],psp->A,psp->sigma);
    }

    return 0;
}

/* ecriture du profil dans un fichier */
int writeProfil(const char *const cFileName, const double *const x, const double *const u, const unsigned int N)
{
    FILE *file=NULL;
    unsigned int i=0;

    file=fopen(cFileName,"a");
    /*fwrite(u,sizeof(*u),N,file);*/
    for(i=1;i<N-1;i++)
        fprintf(file,"%lf\n",u[i]);

    /*fprintf(file,"\n");*/
    fclose(file),file=NULL;

    return 0;
}

int addField(const double *const f1, const double *const f2, double *const fres, const unsigned int N)
{
    unsigned int i;

    for(i=0;i<N;i++)
        fres[i]=f1[i]+f2[i];

    return 0;
}

int addField2(const double *const f1, const double *const f2, double *const fres, const unsigned int N, const double dFac)
{
    unsigned int i;

    for(i=0;i<N;i++)
        fres[i]=f1[i]+dFac*f2[i];

    return 0;
}


/* inversion des tableaux associes aux variables f1 et f2 */
int swapFields(double **f1,double **f2)
{
    double *ptmp;
    ptmp=*f1;
    *f1=*f2;
    *f2=ptmp;

    return 0;
}

/*algorithme d'Euler */
int resol_Euler(const double *const x, const double *const un,  double *const unp1,const sParam *const psp, pfFuncOp pfop)
{
    double *Utmp;
    Utmp=psp->Utmp;
    pfop(x,un,Utmp,psp,psp->dt);
    addField(un,Utmp,unp1,psp->Nx);

    return 0;
}

/*algorithme d'Euler implicite*/
int resol_EulerI(const double *const x, const double *const un,  double *const unp1,const sParam *const psp, pfFuncOp pfop)
{
    invMat1D(un, unp1, psp->Nx, psp->pdA,  psp->pdGam, psp->pdBet);

    return 0;
}

/*algorithme de RK4 */
int resol_RK4(const double *const x, const double *const un,  double *const unp1,const sParam *const psp, pfFuncOp pfop)
{
    double *du1=NULL, *du2=NULL, *du3=NULL, *du4=NULL, *Utmp=NULL;
    unsigned int i=0;


    du1=psp->du1;
    du2=psp->du2;
    du3=psp->du3;
    du4=psp->du4;
    Utmp=psp->Utmp;

    /* calcul des termes */
    pfop(x,un,du1,psp,psp->dt);

    addField2(un,du1,Utmp,psp->Nx,0.5);
    pfop(x,Utmp,du2,psp,psp->dt);

    addField2(un,du2,Utmp,psp->Nx,0.5);
    pfop(x,Utmp,du3,psp,psp->dt);

    addField(un,du3,Utmp,psp->Nx);
    pfop(x,Utmp,du4,psp,psp->dt);

    for(i=0;i<psp->Nx;i++)
    {
        unp1[i]=un[i]+(1./6.)*(du1[i]+2.*du2[i]+2.*du3[i]+du4[i]);
    }

    return 0;
}

/* teste si un fichier existe, renvoie 1 si vrai et 0 si faux */
int isFileExist(const char *const filename)
{
    FILE *file;
    file=fopen(filename,"r");

    if (file==NULL)
        return 0;

    fclose(file);
    return 1;
}

/* cree un nom de fichier a partir d'un prefixe qui sera incremente a chaque fois */
int GenerateFileName(const char *const cFilePrefix, char *const cGenerateName)
{
    char buffer[256]={0};
    unsigned int i=0;

    if ((cFilePrefix==NULL)||(cGenerateName==NULL))
        return 1;

    sprintf(buffer,"%s_%04d.dat",cFilePrefix,i);
    while ((i<10000)&&(isFileExist(buffer)==1))
    {
        i++;
        sprintf(buffer,"%s_%04d.dat",cFilePrefix,i);
    }


    if (i==10000)
        return 1;

    strcpy(cGenerateName,buffer);

    return 0;
}



/* initialisation des matrices d'inversion */
/* c= diagonale superieure */
/* b= diagonale principale */
/* a= diagonale inferieure */
int initInvMat1D(const unsigned int iSizeX, const double *const a, const double *const b, const double *const c, double *const pGam, double *const pBet)
{
    unsigned int k=0;

    pBet[1]=b[1];
    for(k=2;k<iSizeX-1;k++)
    {
        pGam[k] = c[k-1]/pBet[k-1];
        pBet[k]  = b[k]-a[k]*pGam[k];
    }

    for(k=1;k<(iSizeX-1);k++)
        pBet[k]=1./pBet[k];

    return 0;
}

/*routine d'inversion d'une matrice tridiagonale */
int invMat1D(const double *const pSrc, double *const pDest, const unsigned int iSizeX, const double *const a, const double *const pGam, const double *const pBet)
{
    unsigned int k;

    pDest[1]=pSrc[1]*pBet[1];

    for(k=2;k<iSizeX-1;k++)
        pDest[k]=(pSrc[k]-a[k]*pDest[k-1])*pBet[k];

    for(k=(iSizeX-2);k>=1;k--)
        pDest[k]-=pGam[k+1]*pSrc[k+1];

    return 0;
}



/* creation des elements tridiagonaux dans le cas ou l'on ne considere que la diffusion */
int CreateMatrixDiffus(  sParam *const psp)
{
    unsigned int j=0;
    double dCnst=0.0;
    double *a=NULL;
    double *b=NULL;
    double *c=NULL;

    a=psp->pdA;
    b=psp->pdB;
    c=psp->pdC;



    dCnst=psp->C*psp->dt/(psp->dx*psp->dx);
    for(j=1;j<(psp->Nx-1);j++)
    {
        a[j] = -1.*dCnst;
        b[j] = 1+2.*dCnst;
        c[j] = -1.*dCnst;
    }
    c[1]=0.;
    a[psp->Nx-2]=0.;

    return 0;
}

/* creation des elements tridiagonaux dans le cas ou l'on ne considere que l'advection */
int CreateMatrixAdvec(   sParam *const psp)
{
    unsigned int j=0;
    double dCnst=0.0;
    double *a=NULL;
    double *b=NULL;
    double *c=NULL;

    a=psp->pdA;
    b=psp->pdB;
    c=psp->pdC;

    dCnst=psp->V*psp->dt/(psp->dx);
    for(j=1;j<(psp->Nx-1);j++)
    {
        a[j] = -1.*dCnst;
        b[j] = 1+dCnst;
        c[j] = 0.;
    }


    return 0;
}

/* creation des elements tridiagonaux dans le cas ou l'on considere l'advection et la diffusion */
int CreateMatrixAdvDiff( sParam *const psp)
{
    unsigned int j=0;
    double dCnst=0.0;
    double dCnst1=0.0;
    double *a=NULL;
    double *b=NULL;
    double *c=NULL;

    a=psp->pdA;
    b=psp->pdB;
    c=psp->pdC;

    dCnst=psp->V*psp->dt/(psp->dx);
    dCnst1=psp->C*psp->dt/(psp->dx*psp->dx);
    for(j=1;j<(psp->Nx-1);j++)
    {
        a[j] = -1.*(dCnst+dCnst1);
        b[j] = 1+2.*dCnst1+dCnst;
        c[j] = -1.*dCnst1;
    }
    c[1]=0.;
    a[psp->Nx-2]=-1.*dCnst;

    return 0;
}



double sinus(const double x0, const double x, const double A, const double sigma)
{
    double res=0.0;
    double dpi=4.*atan(1.);

    res=A*sin(dpi*(x-x0)*sigma);
    return res;
}

double gaussienne(const double x0, const double x, const double A, const double sigma)
{
    double res=0.0;

    res=A*exp(-((x-x0)*(x-x0))/(sigma*sigma));
    return res;
}

double porte(const double x0, const double x, const double A, const double sigma)
{
    double res=0.0;

    if ((x>(x0-sigma*0.5))&&(x<(x0+sigma*0.5)))
        res=A;

    return res;
}
