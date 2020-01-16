/**
*\file input.h
*\author G. Fuhr
*\version 2.0
*\date 11 Fevrier 2008
*
* fichier d'en tete pour les fonctions de saisies securisees
*
*/
#ifdef __cplusplus
extern "C" {
#endif


#ifndef INPUT_H
#define INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

    /**
    * \enum IO_ENCODING_TYPE
    * \brief enum representant les encodages 32 ou 64b
    */
    enum IO_ENCODING_TYPE{    IO_32B = 32,
        IO_64B = 64,
        IO_UNDEF = 0};
    typedef enum IO_ENCODING_TYPE IO_ENCODING;

    /**
    * \union _IO_VarEnc
    * \brief union pour les conversion de size_t entre 32 et 64b
    */
    typedef union _IO_VarEnc
    {
        unsigned int      ioInt;
        unsigned long     ioLong;
    } IO_VarEnc;

    void clean( char *const buffer, FILE *fp);
    void fpurge(FILE *fp);
    int getInt(void);
    double getDouble(void);
    float getFloat(void);
    long getLong(void);

    char mygetChar(void);
    int isFileExist(const char *const filename);
    int GenerateFileName(const char *const cFilePrefix, char *const cGenerateName);
#endif

#ifdef __cplusplus
}
#endif
