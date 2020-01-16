#include "../headers/myfiles.h"
#include "../../strings/headers/mystrings.h"
#include "../../MemAlloc/headers/MemAlloc.h"


/**
*\def PARAM_NUMBER : nombres de caracteres utilises pour les modes d'ouverture des fichiers
*/
#define PARAM_NUMBER 6

#define LINE_BUFFER_SIZE 256

#define xstr(s) str(s)
#define str(s) #s


static double * local_realloc(double * pointeur, size_t const oldsize,size_t const newsize);


/**
* \fn Fstatic double * local_realloc(double * pointeur, size_t const oldsize,size_t const newsize)
* \brief fonction wrapper du realloc utilisant les routines d'allocation deja developpees, permet de s'affranchir de certaines erreurs sous valgrind
*\param pointeur : pointeur vers la zone memoire dont on veut changer la taille
*\param oldsize : ancienne taille
*\param newsize : nouvelle taille
*\return : adresse de la nouvelle zone memoire ou NULL en cas d'erreur
*/
static double * local_realloc(double * pointeur, size_t const oldsize,size_t const newsize)
{
    static const char FCNAME[] = "local_realloc";
    double *val_tmp=pointeur;

    fprintf(stdout,"reallocation under progress\n");
    if (newsize>oldsize)
    {

        val_tmp=allocArray1D(newsize,sizeof(*val_tmp));
        fprintf(stdout,"reallocation of %p to %p : %ld %ld\n",pointeur,val_tmp,oldsize,newsize);
        if (val_tmp==NULL)
        {
            fprintf(stdout,"reallocation error\n");
            return val_tmp;
        }
        else
        {
            size_t indice=0;
            for(indice=0;indice<oldsize;indice++)
                val_tmp[indice]=pointeur[indice];

            freeArray1D(pointeur),pointeur=NULL;
            pointeur=val_tmp;
        }
    }


    return val_tmp;
}


/**
* \fn FILE *myfopen( char const*const path,  char const*const mode)
* \brief wrapper securise pour la fonction fopen, prend en plus le "mode" "c" pour creer un fichier et permet de renvoyer une erreur en cas d'utilisation de liens symboliques
* Le mode creation de fichier renvoie NULL si le fichier existe deja
*
*\param path : emplacement du fichier et nom
*\param mode : mode d'ouverture du fichier ('r','w','a','b','c','+')
*\return : NULL en cas d'erreur ou un handle vers le fichier ouvert 
*/

FILE *myfopen( char const*const path,  char const*const mode)
{
    FILE *filehandle=NULL;
    char const validparam[PARAM_NUMBER]={'c','r','w','b','a','+'};
    char usedparam[PARAM_NUMBER]={0,0,0,0,0,0};
    size_t const mode_length = mystrlen(mode);
    size_t i=0;
    size_t valid_car=0;
    
#ifdef __linux__
    struct stat fileinfo={0};

    /* check mode validity */
    if (mode_length>3)
        return NULL;

    for (i=0;i<mode_length;i++)
    {
        size_t j=0;
        for(j=0;j<PARAM_NUMBER;j++)
        {
            if (((mode[i])==(validparam[j]))&&(usedparam[j]==0))
            {
                valid_car++;
                usedparam[j]=1;
            }
        }
    }

    if (valid_car!=mode_length)
    {
        return NULL;
    }

    if ((usedparam[0]==1)&&(usedparam[1]==1))
    {
        return NULL;
    }

    /* check file exist or not*/

    if (access(path, F_OK) != 0) /*file doesn't exist */
    {
        if (usedparam[0]==1) /* user want to create file */
        {
            filehandle=fopen(path,"w");
            return filehandle;
        }
        if (usedparam[1]==1) /* user want to read file */
        {
            return NULL;
        }

    }
    else
    {
        if (usedparam[0]==1) /* user want to create file but file already exist*/
        {
            return NULL;
        }
        /* check path correspond to a file and not a symlink or anything else */
        if (stat(path,&fileinfo)!=0)
        {
            return NULL;
        }

        if (S_ISLNK(fileinfo.st_mode)==1)
        {
            return NULL;
        }

        if (fileinfo.st_nlink>1)
        {
            return NULL;
        }


    }
#endif
    filehandle=fopen(path,mode);

    /*return file handle obtained via fopen */
    return filehandle;
}

/**
*\fn int  myfgetline(FILE *filehandle, char *fileLine, size_t *const restrict fileline_size)
*\brief version amelioree du fgetline supportant les depassement de tampon
*\param filename : 
*\param argv : 
*\param pspi : 
*\return : RBM_OK
*/
int  myfgetline(FILE *filehandle, char *fileLine, size_t *const restrict fileline_size)
{
    static const char FCNAME[] = "fgetlineNumber";
    int rbm_er = 0;
    const size_t linesize = 256;
    size_t actualsize=linesize;
    char *linebuffer=allocArray1D(linesize,sizeof(*linebuffer));
    char *result=NULL;

    if (filehandle==NULL)
    {
        rbm_er=-1;
        return rbm_er;
    }


    result=fgets(linebuffer,actualsize,filehandle);
    if (result==NULL)
    {
        rbm_er=-1;
        return rbm_er;
    }
    else
    {
        char *lf = strchr(linebuffer,'\n'); /*recherche du marqueur de fin de ligne dans la chaine */

        if (lf!=NULL)
        {
            *lf='\0';
        }
        else /* le buffer etait trop petit pour contenir toute la ligne*/
        {

            while (lf==NULL)
            {

                linebuffer=realloc(linebuffer,actualsize+linesize);
                if (linebuffer==NULL)
                {
                    rbm_er=-1;
                    return rbm_er;
                }
                result=fgets(linebuffer+actualsize,linesize,filehandle);
                if (result==NULL)
                {
                    rbm_er=-1;
                    free(linebuffer);
                    return rbm_er;
                }
                actualsize+=linesize;
                lf=strchr(linebuffer,'\n');
            }
            *lf='\0';
        }


    }


    if (*fileline_size>actualsize)
    {
        mystrcpy(fileLine,linebuffer,*fileline_size);
    }
    else
    {
        freeArray1D(fileLine);
        fileLine=calloc(actualsize+1,sizeof(*fileLine));
        mystrcpy(fileLine,linebuffer,actualsize);
        *fileline_size=actualsize;
    }


    return rbm_er;
}


/**
*\fn double *myfscanline(char const*const lineToScan, double *val, size_t *NumberValues, size_t *TextInLine)
*\brief 
*\param filename : 
*\param argv : 
*\param pspi : 
*\return : RBM_OK
*/
double *myfscanline(char const*const  lineToScan, size_t *const NumberValues, size_t *const  TextInLine)
{

    static const char FCNAME[] = "myfscanline";
    int rbm_er = 0;
    unsigned int i=0;
    size_t const len = (strlen(lineToScan)>LINE_BUFFER_SIZE?strlen(lineToScan):LINE_BUFFER_SIZE);
    char *tmp=allocArray1D(len,sizeof(*tmp));
    char *restrict buffer_string=allocArray1D(LINE_BUFFER_SIZE+1,sizeof(*buffer_string));
    double *val=allocArray1D(1,sizeof(*val));
    double *val_tmp=NULL;
    char *tmp_dummy=tmp;
    int len_buff=0;
    int len_val=0;
    size_t global_shift=0L;


    mystrcpy(tmp,lineToScan,len);


    *NumberValues=0;
    *TextInLine=0;
    do
    {
        val[i]=0.;
        *buffer_string='\0';
        len_buff=0;
        len_val=0;

        sscanf(tmp,"%"xstr(LINE_BUFFER_SIZE)"[a-zA-Z:#;]%n %lf%n ",buffer_string,&len_buff,&val[i],&len_val);

        /*si notre chaine contient caractere et nombre */
        if (len_val!=0)
        {
            val_tmp=local_realloc(val,i+1,i+2);
            if (val_tmp==NULL)
            {
                rbm_er = 1;
                freeArray1D(tmp);
                freeArray1D(buffer_string);
                freeArray1D(val);
                return NULL;

            }
            val=val_tmp;
            (*NumberValues)++;
            (*TextInLine)++;
            tmp+=(len_val);

        }
        else
        {
            /* si on a juste une chaine de caractere */
            if (len_buff!=0)
            {
                tmp+=(len_buff);
                (*TextInLine)++;
                continue;
            }
            else /* buffer vide ou alors juste un chiffre a lire */
            {
                sscanf(tmp," %lf%n ",val+i,&len_val);
                if (len_val!=0)
                {
                    tmp+=len_val;
                    val_tmp=local_realloc(val,i+1,i+2);
                    if (val_tmp==NULL)
                    {
                        rbm_er = 1;
                        freeArray1D(tmp);
                        freeArray1D(buffer_string);
                        freeArray1D(val);
                        return NULL;

                    }

                    val=val_tmp;
                    (*NumberValues)++;
                }
                else
                {
                    break;
                }
            }

        }

        i++;
        if ((tmp[0]=='\0')||(tmp[0]=='\n'))
            break;


        /*reallocation de memoire pour tmp afin de s'assurer qu'il ai toujours la bonne taille */
        global_shift+=(size_t)tmp - (size_t)tmp_dummy;
        tmp=allocArray1D(LINE_BUFFER_SIZE,sizeof(*tmp));
        mystrcpy(tmp,lineToScan+global_shift,LINE_BUFFER_SIZE);
        freeArray1D(tmp_dummy);
        tmp_dummy=tmp;




    }while(1);

    freeArray1D(tmp_dummy),tmp_dummy=NULL;
    freeArray1D(buffer_string),buffer_string=NULL;


    if (rbm_er!=0)
        freeArray1D(val),val=NULL;

    return val;
}

/**
*\fnint  myGetLineNumber(char const * const restrict filename, unsigned int LineNUmber, unsigned int iMaxLineSize)
*\brief renvoie le nombre de lignes d'un fichier
*\param argc : 
*\param argv : 
*\param pspi : 
*\return : RBM_OK
*/
int  myGetLineNumber(char const * const filename, size_t *LineNumber)
{
    static const char FCNAME[] = "myGetLineNumber";
    int rbm_er = 0;
    size_t NbLines=0;
    size_t linesize=LINE_BUFFER_SIZE;
    char *buffer_line=allocArray1D(LINE_BUFFER_SIZE,sizeof(*buffer_line));
    FILE *fhandle=myfopen(filename,"r");

    do 
    {
        NbLines++;
    } while (myfgetline(fhandle,buffer_line,&linesize)==0);

    fclose(fhandle);
    *LineNumber=NbLines;

    return rbm_er;
}
/*int mygetLineNumber()*/





/**
*\fn RBM_codes  mygetFileExt(char const*const filename, char *const name, char *const fileExt)
*\brief divise le nom du fichier de son extension
*\param  filename : nom du fichier
*\param  : 
*\param  : 
*\return : 0 en cas de succes et 1 si il n'a pas ete possible d'isoler les 2 elements
*/

int  mygetFileExt(char const*const filename, char *const name, char *const fileExt)
{
    static const char FCNAME[] = "mygetFileExt";
/*    size_t const namelength=mystrlen(filename);*/


    int extractval=sscanf(filename,"%[a-zA-Z0-9_].%[a-zA-Z0-9]",name,fileExt);
    if (extractval!=2)
        return 1;

    return 0;
}


