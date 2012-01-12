#ifndef _CNORM_H
    #define _CNORM_H
    #if defined(__STDC__)
        # define PREDEF_STANDARD_C_1989
        # if defined(__STDC_VERSION__)
            #  define PREDEF_STANDARD_C_1990
            #  if (__STDC_VERSION__ >= 199409L)
                #   define PREDEF_STANDARD_C_1994
            #  endif
            #  if (__STDC_VERSION__ >= 199901L)
                #   define PREDEF_STANDARD_C_1999
            #  endif
        # endif
    #endif

    #if !defined(PREDEF_STANDARD_C_1989)
        #define const  
        #define volatile  
    #endif

    #if !defined(PREDEF_STANDARD_C_1999)
        #define inline  
        #define restrict  
    #endif


#endif
