#include "../headers/mystrings.h"

static int changeFormat(char *const format_dest, char const *const format_src, size_t const format_len);

static int changeFormat(char *const format_dest, char const *const format_src, size_t const format_len)
{
    size_t i=0;
    int iPercent=0;

    mystrcpy(format_dest,format_src,format_len);
    /*on remplace les %z par des %l dans les chaines de format version C89*/
    /*for(i=0;i<format_len-1;i++)
    {
        if ((format_dest[i]=='%')&&(format_dest[i+1]=='z'))
        {
            format_dest[i+1]='l';
        }
    }*/

    i=0;
    while(i<format_len-1)
    {
        if (format_dest[i]=='%')
            iPercent=1;
        if ((format_dest[i]=='z')&&(iPercent==1))
        {
            iPercent=0;
            format_dest[i]='l';
        }
        if ((format_dest[i]>='a')&&(format_dest[i]<'z')&&(iPercent==1))
        {
            iPercent=0;
        }
        i++;

    }

    return 0;
}

/**
*\fn size_t mystrlen ( char const*const src)
*\brief calcule la taille d'une chaine de caractere, en supposant que cette chaine n'est constitue que de caracteres imprimables
*\param src : chaine de caractere dont on veut la taille
*\return : "taille" de la chaine
*/
size_t mystrlen ( char const*const src)
{
    size_t dsize=0;

    if (src==NULL)
        return 0L;

    while(1)
    {
        char const c=src[dsize];
        if (!(isprint(c)||c=='\t'||c=='\n'))
        {
            break;
        }
        dsize++;
    }

    return dsize;
}


/**
*\fn char *mystrcpy (char *const des, char const *const src, size_t const dsize)
*\brief copie securisee de chaines de caracteres
*\param des : chaine de destination
*\param src : chaine source
*\param dsize : taille du buffer de destination
*\return : "taille" de la chaine
*/
char *mystrcpy (char *const des
                ,char const *const src
                ,size_t const dsize)
{
    char *s_out = NULL;

    if (des != NULL
        && dsize != 0
        && src != NULL)
    {
        /* longueur de la source */
        size_t const len = strlen (src);

        if (len < dsize - 1)
        {
            /* remplit la fin de 0s */
            strncpy (des, src, dsize);
        }
        else
        {
            /* copie */
            memcpy (des, src, dsize - 1);

            /* marquage de la fin */
            des[dsize - 1] = 0;
        }
        s_out = des;
    }

    return s_out;
}




/**
*\fn int mystrstr(char const*const stringtoanalyse, char const*const substring, char *const result, long const LenResult)
*\brief extrait d'une chaine de caractere la partie presente avant la premiere occurence de substring
*\param str : tableau de caractere contenant la chaine formatee
*\param size : taille de la chaine str
*\param format : liste d'arguments
*\return : nombre de caracteres copies dans str sans compter le \\0 final ou qui auraient du etre copies si la chaine avait ete assez longue
*/
long mystrstr(char const*const stringtoanalyse, char const*const substring, char *const result, long const LenResult)
{
long const len=strstr(stringtoanalyse,substring)-stringtoanalyse;
long i=0;
long secure_len=LenResult-1<len?LenResult-1:len;
   
  for(i=0;i<secure_len;i++)
      result[i]=stringtoanalyse[i];

  result[secure_len]='\0';
  return len;  
}

/**
*\fn int mysnprintf (char *str, size_t size, const char  *format,...)
*\brief wrapper pour la fonction snprintf
*\param str : tableau de caractere contenant la chaine formatee
*\param size : taille de la chaine str
*\param format : liste d'arguments
*\return : nombre de caracteres copies dans str sans compter le \\0 final ou qui auraient du etre copies si la chaine avait ete assez longue
*/
int mysnprintf (char *str, size_t size, const char  *format,...)
{

    va_list ap;
    int returnval =0;

#ifdef PREDEF_STANDARD_C_1999
    va_start(ap, format);
    returnval= vsnprintf(str,size,format,ap);
    va_end(ap);
    return returnval;

#else
    const size_t len_format=mystrlen(format)+1;
    char *newformat = allocArray1D(len_format,sizeof(*newformat));
    va_start(ap, format);

    changeFormat(newformat,format, len_format);


    if (len_format>size)
    {
        va_end(ap);
        str[0]='\0';
        freeArray1D(newformat),newformat=NULL;
        return -1;
    }

    returnval=vsprintf(str,newformat,ap);
    freeArray1D(newformat),newformat=NULL;
    va_end (ap);

    return returnval;
#endif
}


/**
*\fn int myfprintf(FILE *stream,const char  *format,...)
*\brief wrapper pour la fonction fprintf
*\param stream : flux
*\param format : chaine de formatage
*\param ... : liste d'arguments
*\return : nombre de caracteres ecrits dans le flux
*/
int myfprintf(FILE *stream,const char  *format,...) 
{
    va_list ap;
    const size_t len_format=mystrlen(format)+1;
    int returnval =0;


#ifdef PREDEF_STANDARD_C_1999
    /*fprintf(stdout,"C99\n");*/
    va_start(ap, format);
    returnval=vfprintf(stream,format,ap);
    va_end (ap);
    return returnval;
#else
    /*recherche des indicateurs de "size_t" et on les convertit*/
    char *newformat = allocArray1D(len_format,sizeof(*newformat));


    va_start(ap, format);
    changeFormat(newformat,format, len_format);
    returnval=vfprintf(stream,newformat,ap);
    freeArray1D(newformat),newformat=NULL;
    va_end (ap);
    return returnval;
#endif
}



/**
*\fn int myprintf(const char  *format,...)
*\brief wrapper pour la fonction printf
*\param format : chaine de formatage
*\param ... : liste d'arguments
*\return : nombre de caracteres ecrits
*/
int myprintf(const char  *format,...)
{
    const size_t len_format=mystrlen(format)+1;
    int returnval =0;

    va_list ap;
    va_start(ap, format);

#ifdef PREDEF_STANDARD_C_1999
    returnval= vfprintf(stdout,format,ap);
    va_end (ap);
    return returnval;
#else
    /*recherche des indicateurs de "size_t" et on les convertit*/
    {
        char *newformat = allocArray1D(len_format,sizeof(*newformat));


        changeFormat(newformat,format, len_format);

        returnval=vfprintf(stdout,newformat,ap);
        freeArray1D(newformat),newformat=NULL;
        va_end (ap);

        return returnval;
    }

#endif
}
