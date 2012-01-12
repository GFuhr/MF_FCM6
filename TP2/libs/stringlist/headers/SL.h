/**
*\file SL.h
*\author G. Fuhr
*\version 2.0
*\date 11 Fevrier 2008
*
* fichier d'en-tete pour les fonctions du  perser de fichier ini
*
*/

#ifdef __cplusplus
 extern "C" {
#endif

#ifndef SL_SL_H
#define SL_SL_H
#include <stdlib.h>        
#include <stdio.h>
#include <string.h>
#include "../../inputs/headers/input.h"
#include "../../strings/headers/mystrings.h"
#include "../../files/headers/myfiles.h"

#define SL_HDF5
#include "hdf5.h"


/**
 * \enum SL_ID_CODE
 * \brief enum contenant les types possibles de donnees
 *
 * SL_ID contient les identifiants pour les types de donnees pouvant etre utilises
 */
enum SL_ID_CODE{    SL_FLOAT,
                    SL_DOUBLE,        
                    SL_INT,           
                    SL_CHAR,         
                    SL_STRING,       
                    SL_LONG,         
                    SL_SIZE_T,
                    SL_UNDEF};
typedef enum SL_ID_CODE SL_ID;

#define SL_CHAR_FLOAT  "float"
#define SL_CHAR_DOUBLE "double"
#define SL_CHAR_INT    "int"
#define SL_CHAR_CHAR   "char"
#define SL_CHAR_STRING "string"
#define SL_CHAR_LONG   "long"
#define SL_CHAR_SIZE_T "size_t"




/**
 * \enum SL_ID_ERROR
 * \brief enum contenant les types possibles de donnees
 *
 * SL_ID contient les identifiants pour les types de donnees pouvant etre utilises
 */
enum SL_ID_ERROR{SL_OK, 
                    SL_ERROR,                 /*! < Une erreur est survenue*/
                    SL_ERROR_FILE_EXIST,      /*! < Tentative de creation d'un fichier deja existant*/
                    SL_ERROR_READ,            /*! < Erreur de lecture*/
                    SL_ERROR_WRITE,           /*! < Erreur d'ecriture*/
                    SL_ERROR_OPEN_FILE,       /*! < Erreur a l'ouverture du fichier */
                    SL_ERROR_FILE_NOT_OPENED, /*! < Tentative d'utilisation d'un fichier sans l'ouvrir*/
                    SL_ERROR_UNKNOWN_DATA,    /*! < Type de donnees non reconnues*/
                    SL_ERROR_VAR_NOT_FOUND,   /*! < Variable non trouvee*/
                    SL_ERROR_TYPE_MISMATCH,   /*! < Types differents*/
                    SL_ERR0R_SL_NOT_INITIALISED,
                    SL_ERROR_MEMORY};      /*! < Utilisation d'une liste non initialisee */

typedef enum SL_ID_ERROR SL_CODE;

/**
 * \union SL_Var
 * \brief union pour l'affectation des variables en provenance du fichier
 *
 * SL_ID contient les identifiants pour les types de donnees pouvant etre utilises
 */
typedef union _SL_VarType
   {
   double   slDouble;
   int      slInt;
   char     slChar;
   char     *slString;
   long     slLong;
   float    slFloat;
   size_t   slSize_t;
   } SL_VarType;

typedef struct _SL_Var
{
    SL_VarType value;
    SL_ID type;
}SL_Var;

/*others*/
/**
*\def BUFFER_SIZE
* taille maximale possible pour les chaines de caractere
*/
#define BUFFER_SIZE 1024


/**
*\struct SLelement 
*\brief Structure contenant les elements du menu
*/
typedef struct _SLelement
{
    struct _SLelement *NextElm;     /*! < pointeur vers l'element suivant, NULL par defaut */
    struct _SLelement *PrevElm;    /*! < pointeur vers l'element precedent, NULL par defaut */
    char *sVarName;                   /*! < nom associe a l'element */
    SL_Var Var;                 /*! < type et contenu de la variable*/

}SLelement,*psSLelement;

/**
*\struct sSL 
*\brief Structure de base de la liste chainee contenant la liste des variables
*/
typedef struct _SL
{
    SLelement *pSLfirst;    /*! < pointeur vers le premier element de la liste, NULL par defaut */
    SLelement *pSLlast;     /*! < pointeur vers le dernier element, NULL par defaut */
    SLelement *pSLcurrent;  /*! < pointeur vers le dernier element utilise, NULL par defaut */
    FILE *file;              /*! < pointeur vers le fichier associe a la liste, NULL par defaut */
#ifdef SL_HDF5
    hid_t file_hdf5;
#endif
}sSL, *psSL;


/*fonctions exportees */
sSL *SL_malloc(size_t const count);
void SL_free(psSL pssl);
SL_CODE SL_fopen(psSL const pssl,char const * const filename,char const * const mode);
SL_CODE SL_fclose(psSL const pssl);
SL_CODE SL_menuVar(psSL const pssl);
SL_CODE SL_printVar(psSL pssl,char const * const cVarName);
SL_CODE SL_printAllVar(psSL pssl);
SL_CODE SL_getVar(psSL pssl,char const * const cVarName,void *const pvValue);
SL_CODE SL_scanVar(psSL pssl,char const *const cVarName,SL_ID iVarType, void *pvVal);
SL_CODE SL_writeFile(psSL const pssl);
SL_CODE SL_readFile(psSL const pssl);
SL_CODE SL_readVar(psSL const pssl,char const * const cVarName,const SL_ID slVarT, void *pvVal);
SL_CODE SL_putVar(psSL const pssl,char const * const cVarName,SL_Var const slVar);


#ifdef SL_HDF5
SL_CODE     SL_writeFileHDF5(psSL const pssl,char const*const groupname);
SL_CODE     SL_readFileHDF5(psSL const pssl,char const*const groupname);
herr_t      op_stringlist( hid_t g_id, const char *name, const H5L_info_t *info, void *op_data);
hid_t       SLToHDF5Type(SL_ID SLvarType);
SL_ID  HDF5ToSlType(hid_t SLvarType);
SL_CODE     SL_closeHDF5File( psSL stringlist);
SL_CODE     SL_openHDF5File(psSL stringlist, char const*const filename);
SL_CODE     isObjectExist(psSL stringlist,char const*const objectname);
#endif

#endif
#ifdef __cplusplus
}
#endif
