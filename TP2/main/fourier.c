

static void transposefieldR(double const*const F, double *const Utrans, sFieldSize const*const sFs)
{
    long x=0;
    long y=0;
    long oldpos,newpos;
    for(y=0;y<sFs->iSize2;y++)
    {
        for(x=0;x<sFs->iSize1;x++)
        {
            oldpos = x+sFs->iSize1*y;
            newpos = y+sFs->iSize2*x;
            Utrans[newpos]=F[oldpos];
        }
    }
}



static void transposefieldC(double const*const F, double *const Utrans, sFieldSize const*const sFs)
{
    long x=0;
    long y=0;
    long oldpos,newpos;
    long const ImPos=sFs->iSize1*sFs->iSize2;
    for(y=0;y<sFs->iSize2;y++)
    {
        for(x=0;x<sFs->iSize1;x++)
        {
            oldpos = x+sFs->iSize1*y;
            newpos = y+sFs->iSize2*x;
            Utrans[newpos]=F[oldpos];
            Utrans[newpos+ImPos]=F[oldpos+ImPos];
        }
    }
}