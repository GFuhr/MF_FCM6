#include "../include/common.h"
#include "../include/H2D_Func.h"
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
        resultat = 0.0*exp(-((x-0.5*LX)*(x-0.5*LX)/(10.*LX)+(y-0.005*LY)*(y-0.005*LY)/(0.001*LY)));
    }
    /*ou partie imaginaire en fonction de dComplex*/
    else
    {
        resultat=0;
    }

    return resultat;
}




/**
*\fn double  FuncInit(double const x, double const y)
*\brief calcule la condition initiale implémentée dans H2D
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

                resultat = exp(-((x-0.5*LX)*(x-0.5*LX)/(20.*LX)+(y-0.5*LY)*(y-0.5*LY)/(20*LY)));
    }
    /*ou partie  imaginaire*/
    else
    {
                resultat = exp(-((x-0.5*LX)*(x-0.5*LX)/(20.*LX)+(y-0.5*LY)*(y-0.5*LY)/(20*LY)));
    }
    


    return resultat;
}