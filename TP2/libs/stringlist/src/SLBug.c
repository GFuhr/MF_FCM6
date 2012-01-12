#include <stdio.h>
#include <string.h>

enum SL_ID_CODE{    SL_FLOAT,
                    SL_DOUBLE,        
                    SL_INT,           
                    SL_CHAR,         
                    SL_STRING,       
                    SL_LONG,         
                    SL_UNDEF};
typedef enum SL_ID_CODE SL_ID;

#define SL_CHAR_FLOAT  "float"
#define SL_CHAR_DOUBLE "double"
#define SL_CHAR_INT    "int"
#define SL_CHAR_CHAR   "char"
#define SL_CHAR_STRING "string"
#define SL_CHAR_LONG   "long"

/**
 * \enum SL_ID_ERROR
 * \brief codes retour possibles
 *
 */
enum SL_ID_ERROR{SL_OK, 
                    SL_ERROR,                 /*! < Une erreur est survenue*/
                    SL_ERROR_FILE_EXIST,      /*! < Tentative de creation d'un fichier deja existant*/
                    SL_ERROR_READ,            /*! < Erreur de lecture*/
                    SL_ERROR_WRITE,           /*! < Erreur d'ecriture*/
                    SL_ERROR_OPEN_FILE,       /*! < Erreur a l'ouverture du fichier */
                    SL_ERROR_FILE_NOT_OPENED, /*! < Erreur de flux*/
                    SL_ERROR_UNKNOWN_DATA,    /*! < Type de donnees non reconnues*/
                    SL_ERROR_VAR_NOT_FOUND,   /*! < Variable non trouvee*/
                    SL_ERROR_TYPE_MISMATCH,   /*! < Types differents*/
                    SL_ERR0R_SL_NOT_INITIALISED};      /*! < Utilisation d'une liste non initialisee */

typedef enum SL_ID_ERROR SL_CODE;

/**
 * \union SL_VarType
 * \brief stockage de la valeur lue dans le fichier en fonction de son type
 *
 */
typedef union _SL_VarType
   {
   double   slDouble;
   int      slInt;
   char     slChar;
   char     *slString;
   long     slLong;
   float    slFloat;
   } SL_VarType;

typedef struct _SL_Var
{
    SL_VarType value;
    SL_ID type;
}SL_Var;


typedef struct _StringList
{
    SL_Var contenu;
    FILE *file;              
}StringList;

SL_CODE SL_readFile(StringList const *Liste);

/*lecture d'un fichier, fichier suppose ouvert par une autre fonction */
SL_CODE SL_readFile(StringList const *Liste)
{
    SL_CODE sl_er=SL_OK;
    char buffer[1024];
    char *res=NULL;
    SL_Var element;


    if (Liste->file == NULL)
    {
        sl_er = SL_ERROR_FILE_NOT_OPENED;
        return sl_er;
    }
    rewind(Liste->file);  



    while(feof(Liste->file)==0)
    {
        res=fgets(buffer, sizeof buffer, Liste->file);
        
        if (res==NULL)
        {
            sl_er = SL_ERROR;
            break;
        }

        /*type de donnee */
        if (strncmp(buffer,SL_CHAR_INT,sizeof SL_CHAR_INT ) ==0) 
            {element.type=SL_INT;}
        else if (strncmp(buffer, SL_CHAR_DOUBLE ,sizeof SL_CHAR_DOUBLE ) ==0)  
            {element.type=SL_DOUBLE;}
        else if (strncmp(buffer, SL_CHAR_FLOAT  ,sizeof SL_CHAR_FLOAT )  ==0)  
            {element.type=SL_FLOAT;}
        else if (strncmp(buffer, SL_CHAR_LONG   ,sizeof SL_CHAR_LONG )   ==0)  
            {element.type=SL_LONG;}
        else if (strncmp(buffer, SL_CHAR_CHAR   ,sizeof SL_CHAR_CHAR )   ==0)  
            {element.type=SL_CHAR;}
        else if (strncmp(buffer, SL_CHAR_STRING ,sizeof SL_CHAR_STRING ) ==0)  
            {element.type=SL_STRING;}
        else
        {
            sl_er = SL_ERROR_UNKNOWN_DATA;
        }
   }

    return sl_er;
}
