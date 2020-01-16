/**
*\file myfiles.h
*\author G. Fuhr
*\version 2.0
*\date 11 Fevrier 2008
*
* fichier d'en tete pour les fonctions d'ouverture securisees des fichiers
*
*/
#ifdef __cplusplus
extern "C" {
#endif


#ifndef MYFILES_H
#define MYFILES_H


#include <stdio.h>
#include <malloc.h>
#ifdef __linux__
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include "../../inputs/headers/input.h"

FILE *myfopen( char const*const path,  char const*const mode);


int    fgetlineNumber(char const*const filename, int *iLineNumber, int *iLineSize);

int  myfgetline(FILE *filehandle, char *fileLine, size_t *const  fileline_size);

double *myfscanline(char const*const lineToScan, size_t *const NumberValues, size_t *const  TextInLine);

int  myGetLineNumber(char const * const filename, size_t *LineNumber);

int  mygetFileExt(char const*const filename, char *const name, char *const fileExt);

#endif

#ifdef __cplusplus
}
#endif