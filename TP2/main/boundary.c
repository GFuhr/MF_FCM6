
#include "../include/H2D_types.h"
#include "../include/H2D_Func.h"

/*conditions de bords type dirichlet avec points fantomes*/
void fourier_bc(double *const field,sFieldSize const*const sFs)
{
    long y=0;
    long const offset = sFs->iSize1*sFs->iSize2;
    for (y=0;y<sFs->iSize2;y++)
    {
        field[y*sFs->iSize1]       = -field[y*sFs->iSize1+2]; /*f[0]=-f[2] pour tout y*/
        field[(y+1)*sFs->iSize1-1] = -field[(y+1)*sFs->iSize1-3];/*f[Nx+1]=-f[Nx-1] pour tout y*/

        field[offset+y*sFs->iSize1]       = -field[offset+y*sFs->iSize1+2]; /*f[0]=-f[2] pour tout y*/
        field[offset+(y+1)*sFs->iSize1-1] = -field[offset+(y+1)*sFs->iSize1-3];/*f[Nx+1]=-f[Nx-1] pour tout y*/

    }
}


/*conditions de bords type dirichlet avec points fantomes en x et periodiques en y*/
void real_bc(double *const field,sFieldSize const*const sFs)
{
    long y=0,x=0;


    for (x=0;x<sFs->iSize1;x++)
    {
        field[x]       = field[sFs->iSize1*(sFs->iSize2-2)+x]; /*f[0]=-f[2] pour tout y*/
        field[sFs->iSize1*(sFs->iSize2-1)+x] = field[sFs->iSize1+x];/*f[Nx+1]=-f[Nx-1] pour tout y*/
    }

    for (y=0;y<sFs->iSize2;y++)
    {
        field[y*sFs->iSize1]       = -field[y*sFs->iSize1+2]; /*f[0]=-f[2] pour tout y*/
        field[(y+1)*sFs->iSize1-1] = -field[(y+1)*sFs->iSize1-3];/*f[Nx+1]=-f[Nx-1] pour tout y*/
   }


}


/*conditions de bords type dirichlet avec points fantomes en x et periodiques en y*/
void null_bc(double *const field,sFieldSize const*const sFs)
{
    long y=0,x=0;
/*fonction a remplir*/

}

