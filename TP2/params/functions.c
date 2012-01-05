#include "../include/common.h"
#include "../params/params.h"



/**
*\fn double  source(double const x, double const y)
*\brief calcul du terme de source
*\param  : 
*\param  : 
*\param  : 
*\return : RBM_OK
*/
double  source(double const x, double const y, int const dComplex)
{
    static const char FCNAME[] = "source";
    double resultat=0;

    /*partie reelle*/
    if (dComplex==0)

    {
        resultat = 2.5*exp(-((x-0.5*LX)*(x-0.5*LX)+(y-0.5*LY)*(y-0.5*LY))/(100*LX));
    }
    /*ou partie reelle en fonction de dComplex*/
    else
    {
        resultat=0;
    }

    return resultat;
}




/**
*\fn double  FuncInit(double const x, double const y)
*\brief 
*\param  : 
*\param  : 
*\param  : 
*\return : RBM_OK
*/
double  FuncInit(double const x, double const y, int const dComplex)
{
    static const char FCNAME[] = "FuncInit";
    double resultat=0;

    /*partie reelle*/
    if (dComplex==0)

    {
        resultat=sin(x*M_PI/LX);
    }
    /*ou partie  imaginaire*/
    else
    {
        resultat=sin(x*M_PI/LX);
    }


    return resultat;
}