/**
*\file mystrings.h
*\author G. Fuhr
*\version 2.0
*\date 11 Fevrier 2008
*
* fichier d'en tete pour les fonctions de gestion des chaines de caracteres
*
*/
#ifdef __cplusplus
extern "C" {
#endif


#ifndef MYSTRINGS_H
#define MYSTRINGS_H

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "../../MemAlloc/headers/MemAlloc.h"
#include "../../C_Norm/C_Norm.h"

size_t mystrlen ( char const*const src);
char *mystrcpy (char *const des, char const *const src, size_t const dsize);
#ifdef __GNUC__
int mysnprintf (char *str, size_t size, const char  *format,...) __attribute__((format(printf, 3, 4)));
int myfprintf(FILE *file, const char  *format,...) __attribute__((format(printf, 2, 3))); 
int myprintf( const char  *format,...) __attribute__((format(printf, 1, 2)));
#else
int mysnprintf (char *str, size_t size, const char  *format,...);
int myfprintf(FILE *file, const char  *format,...);
int myprintf( const char  *format,...);
#endif
long mystrstr(char const*const stringtoanalyse, char const*const substring, char *const result, long const LenResult);

#endif

#ifdef __cplusplus
}
#endif
