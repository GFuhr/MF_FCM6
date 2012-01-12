/**
*\file rbm_types.h
    *\author G. Fuhr
*\version 2.0
*\date 11 Fevrier 2008
*
* fichier d'en tetes pour les differents types de donnees presentes dans EMEDGE3D
*
*/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef RBM_TYPES_H
#define RBM_TYPES_H
/*#include "config.h"*/
#include <stdlib.h>


#define _CINT_ const int
#define _CUINT_ const unsigned int
#define H2D_MAX_PATH 1024

#ifndef M_PI
#define  M_PI 3.1415926535897932
#endif



#define H2D_FALSE 0
#define H2D_TRUE 1

#ifndef EPS
#define EPS 1e-6
#endif

#define NULLFILENAME "null"



typedef enum { _X,_Y,_XF,_XY } H2D_INV_DIRECT;

typedef enum { DX,DY,DT,KX,KY,ETA } H2D_param_enum;
typedef enum { _EULE,_EULI,_RK2,_RK4,_CN } H2D_scheme;

typedef enum { _DIR,_NM,_PER } H2D_BC;


typedef  double *pField1D;
typedef  double **pField2D;
typedef  double **pField1Dc;


typedef double *const pcnstField1D;
typedef double *const *const pcnstField2D;
typedef double *const *const pcnstField1Dc;


typedef double const *const pcnstField1D_cnst;
typedef double const *const *const pcnstField2D_cnst;
typedef double const *const *const pcnstField1Dc_cnst;




/**
*\struct sFieldSize
    *\brief Structure contenant la taille d'un champ suivant les 3 dimensions
*/
typedef struct _sFieldSize
{
    long iSize1;      /*! < dimension suivant la direction 1, typiquement x */
    long iSize2;      /*! < dimension suivant la direction 2, typiquement y */
    int iSizeC;      /*! < vaut 1 pour un champs reel et 2 pour un champs complexe */
}sFieldSize, *psFieldSize;

/**
*\struct sFieldDiscret
    *\brief Structure contenant les informations de discretisation spatiale pour un champ
*/
typedef struct _sFieldDiscret
{
    double dDx1;        /*! < valeur du pas suivant la direction 1 */
    double dDx2;        /*! < valeur du pas suivant la direction 2 */
    double dKx1;        /*! < valeur du pas suivant la direction 3 */
    double dKx2;        /*! < valeur du pas suivant la direction 3 */
}sFieldDiscret, *psFieldDiscret;



/**
*\struct sFieldInfo
    *\brief Structure contenant les informations  pour un champ (taille et discretisation)
*/
typedef struct _sFieldInfo
{
    sFieldSize sSize;
    sFieldDiscret sStep;
}sFieldInfo, *psFieldInfo;






/**
*\struct sRK4Field
    *\brief Structure contenant les champs aux differents pas de temps
*/
typedef struct sRK4Field
{
    double *k1;          /*! < champs intermediaire k1 */
    double *k2;          /*! < champs intermediaire k2 */
    double *k3;          /*! < champs intermediaire k3 */
    double *k4;          /*! < champs intermediaire k4 */
}sRK4Field, *psRK4Field;








/**
*\struct sCurveOp
    *\brief Structure contenant les constantes a utiliser pour l'operateur de courbure
*/
typedef struct sGeomOp
{
    double    dRFactor;
    double    dRmin; 
    double    *pdRadius;  /*! tableau contenant les valeurs de m*ky */
    size_t       iSize;    
}sGeomOp, *psGeomOp;






/**
*\struct sLaplacianOp
    *\brief Structure contenant les constantes a utiliser pour l'operateur  du laplacien
*/
typedef struct sLaplacianOp
{
    double    * pdCnstGradp;
    double    * pdCnstGradm;
    double    **ppdCnst;
    double    dFactor;
    size_t       iSizeX;
    size_t       iSizeY;
}sLaplacianOp, *psLaplacianOp;




/**
*\struct _sInvLap
    *\brief Structure contenant les matrices utilises pour l'inversion du laplacien
*/
typedef struct _sInvLap
{

    pField1D    a;              /*! < tableau "a" de la routine tridiag du numerical recipes utilise pour l'inversion du laplacien */
    pField1D    gam;            /*! < tableau "gam" de la routine tridiag du numerical recipes utilise pour l'inversion du laplacien */
    pField1D    bet;            /*! < tableau "bet" de la routine tridiag du numerical recipes utilise pour l'inversion du laplacien */
    H2D_INV_DIRECT dir;
    pField1D    z; 
    pField1D    u; 
    double beta;
    double gamma;
}sInvLap,*psInvLap;



typedef struct sConst
{
    double    dDt;                   /*! < pas de temps */

    double    dEta;                  /*! < kz^2 */

    long Tmax;
    long TIO;


    sGeomOp    *sGeom;
    sLaplacianOp * sLapTor;         /*! < calcul du laplacien en fonction du type de coordonnees*/

    sFieldSize * sSize;     /*! <  taille globale du champs */
    sFieldDiscret * sSteps;      /*! < valeur des pas dx,ky,dz dans l'espace de fourier */

    pField1D pdSource;                  /*! < champ 1D contenant le profil de source*/

    pField1D pInit;                  /*! < champ 1D contenant le profil de source*/

    pField1D pdMKy;                      /*! < tableau 1D contenant les valeur de m*ky pour O<m<Mmax */


    double *pUrhs;
    double *pUtmp;

    double *pUk1;
    double *pUk2;
    double *pUk3;
    double *pUk4;

    sInvLap *sInvX;
    sInvLap *sInvY;

    H2D_INV_DIRECT eDir;

}sConst,*psConst;

typedef void (*pfBC)         (double *const ,sFieldSize const*const);
/*typedef int (*pfFuncTime)   (const double *const , const double *const ,  double *const , const sConst *const, pfBC);*/

typedef int (*pfLinearOp)   ( double const*const ,  double const*const ,  sConst const  *const);


typedef int (*pfstep)         (double *const ,double const*const , sConst const*const, pfLinearOp, pfBC );

#endif


#ifdef __cplusplus
}
#endif


