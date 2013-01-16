
#include "../include/H2D_types.h"
#include "../include/H2D_Func.h"

/* dirichlet boundary with ghost points*/
void fourier_bc(double *const field,sFieldSize const*const sFs)
{
    long y=0;
    long const offset = sFs->iSize1*sFs->iSize2;
    for (y=0;y<sFs->iSize2;y++)
    {
        field[y*sFs->iSize1]       = -field[y*sFs->iSize1+2]; /*f[0]=-f[2] for each y*/
        field[(y+1)*sFs->iSize1-1] = -field[(y+1)*sFs->iSize1-3];/*f[Nx+1]=-f[Nx-1] for each y*/

        field[offset+y*sFs->iSize1]       = -field[offset+y*sFs->iSize1+2]; /*f[0]=-f[2] for each y*/
        field[offset+(y+1)*sFs->iSize1-1] = -field[offset+(y+1)*sFs->iSize1-3];/*f[Nx+1]=-f[Nx-1] for each y*/

    }
}


/*dirichlet boundary with ghost points in x and periodic in y*/
void real_bc(double *const field,sFieldSize const*const sFs)
{
    long y=0,x=0;


    for (x=0;x<sFs->iSize1;x++)
    {
        field[x]       = field[sFs->iSize1*(sFs->iSize2-2)+x]; /*f[0]=-f[2] for each y*/
        field[sFs->iSize1*(sFs->iSize2-1)+x] = field[sFs->iSize1+x];/*f[Nx+1]=-f[Nx-1] for each y*/
    }

    for (y=0;y<sFs->iSize2;y++)
    {
        field[y*sFs->iSize1]       = -field[y*sFs->iSize1+2]; /*f[0]=-f[2] for each y*/
        field[(y+1)*sFs->iSize1-1] = -field[(y+1)*sFs->iSize1-3];/*f[Nx+1]=-f[Nx-1] for each y*/
   }


}


/*Von Neuman  boundary with ghost points*/
void null_bc(double *const field,sFieldSize const*const sFs)
{
    long y=0,x=0;
/* type code here */

}

