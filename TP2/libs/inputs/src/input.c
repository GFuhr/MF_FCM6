#include "../headers/input.h"
#include "../../MemAlloc/headers/MemAlloc.h"
/**
*\file input.c
*\author G. Fuhr
*\version 2.0
*\date 11 Fevrier 2008
*
* fichier contenant les fonctions pour des saisise securisees
*
*/

/**
* \fn void clean(const char *buffer, FILE *fp)
* \brief vidage du buffer associe a un flux
*\param buffer
*\param fp : flux a purger
*/
void clean( char *const buffer, FILE *fp)
{
    char *p = strchr(buffer,'\n');
    if (p != NULL)
        *p = 0;
    else
    {
        int c;
        while ((c = fgetc(fp)) != '\n' && c != EOF);
    }
}

/**
* \fn static void purge(int val)
* \brief nettoyage du buffer d'entree apres une saisie
* myprintf ("Entrez un caractere: ");
* scanf ("%c", &num);
* purge(num)
*
*
*\param val : valeur a partir de laquelle purger le buffer
*/
static void purge(int val)
{
    if (val != '\n')
    {
        int c;
        while ((c = getchar()) != '\n')
        {
        }
    }
}

/**
* \fn void fpurge(FILE *fp)
* \brief nettoyage du flux entrant pointe par fp
*\param fp : flux a vider
*/
void fpurge(FILE *fp)
{
    if (fp != NULL)
    {
        int c;

        while ((c = fgetc(fp)) != '\n' && c != EOF)
        {
        }
    }
}



/**
* \fn int getInt(void)
* \brief utilisation securisee de scanf pour la saisie d'un entier
*\return le nombre entier saisi
*/
int getInt(void)
{
    int nombre=0;
    int isOk=0;
    int retour=0;

    while (!isOk){

        retour = scanf("%d%*[^\n]", &nombre);

        if ( !retour ){
            /* erreur de saisie, on vide le flux */
            int c;
            while ( ((c = getchar()) != '\n') && c != EOF);


        }
        else {
            /* reussite de la saisie */
            getchar(); /* on enleve le '\n' restant */


            isOk = 1;  /* sort de la boucle */
        }

    }

    return nombre;
}

/**
* \fn int getInt(void)
* \brief utilisation securisee de scanf pour la saisie d'un entier
*\return le nombre entier saisi
*/
char mygetChar(void)
{
    char nombre=0;
    int isOk=0;
    int retour=0;

    while (!isOk){

        retour = scanf("%c%*[^\n]", &nombre);

        if ( !retour ){
            /* erreur de saisie, on vide le flux */
            int c;
            while ( ((c = getchar()) != '\n') && c != EOF);


        }
        else {
            /* reussite de la saisie */
            getchar(); /* on enleve le '\n' restant */


            isOk = 1;  /* sort de la boucle */
        }

    }

    return nombre;
}


/**
* \fn double getDouble(void)
* \brief utilisation securisee de scanf pour la saisie d'un double
*\return le nombre decimal saisi
*/
double getDouble(void)
{
    double nombre=0;
    int isOk=0;
    int retour=0;

    while (!isOk){

        retour = scanf("%lf%*[^\n]", &nombre);

        if ( !retour ){
            /* erreur de saisie, on vide le flux */
            int c;
            while ( ((c = getchar()) != '\n') && c != EOF);


        }
        else {
            /* reussite de la saisie */
            getchar(); /* on enleve le '\n' restant */

            isOk = 1;  /* sort de la boucle */
        }
    }
    return nombre;
}


/**
* \fn float getFloat(void)
* \brief utilisation securisee de scanf pour la saisie d'un float
*\return le nombre decimal saisi
*/
float getFloat(void)
{
    float nombre=0;
    int isOk=0;
    int retour=0;

    while (!isOk){

        retour = scanf("%f%*[^\n]", &nombre);

        if ( !retour ){
            /* erreur de saisie, on vide le flux */
            int c;
            while ( ((c = getchar()) != '\n') && c != EOF);


        }
        else {
            /* reussite de la saisie */
            getchar(); /* on enleve le '\n' restant */

            isOk = 1;  /* sort de la boucle */
        }
    }
    return nombre;
}

/**
* \fn double getLong(void)
* \brief utilisation securisee de scanf pour la saisie d'un long int
*\return le nombre entier saisi
*/
long getLong(void)
{
    long nombre=0L;
    int isOk=0;
    int retour=0;

    while (!isOk){

        retour = scanf("%ld%*[^\n]", &nombre);

        if ( !retour ){
            /* erreur de saisie, on vide le flux */
            int c;
            while ( ((c = getchar()) != '\n') && c != EOF);


        }
        else {
            /* reussite de la saisie */
            getchar(); /* on enleve le '\n' restant */

            isOk = 1;  /* sort de la boucle */
        }
    }
    return nombre;
}





/* teste si un fichier existe, renvoie 1 si vrai et 0 si faux */
int isFileExist(const char *const filename)
{
    FILE *file=fopen(filename,"r");

    if (file==NULL)
        return 0;

    fclose(file);
    return 1;
}


/* cree un nom de fichier a partir d'un prefixe qui sera incremente a chaque fois */
int GenerateFileName(const char *const cFilePrefix, char *const cGenerateName)
{   
    char buffer[256]={0};
    unsigned int i=0;

    if ((cFilePrefix==NULL)||(cGenerateName==NULL))
        return 1;

    sprintf(buffer,"%s_%04d.dat\0",cFilePrefix,i);
    while ((i<10000)&&(isFileExist(buffer)==1))
    {
        i++;
        sprintf(buffer,"%s_%04d.dat\0",cFilePrefix,i);

    }


    if (i==10000)
        return 1;

    strcpy(cGenerateName,buffer);

    return 0;
}


