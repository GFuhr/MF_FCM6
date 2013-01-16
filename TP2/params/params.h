#ifdef __cplusplus
extern "C" {
#endif 

#ifndef H2D_PARAMS_H
#define H2D_PARAMS_H

#include "../include/common.h"
#ifndef M_PI
#define M_PI 3.141592653589793
#endif

#define C 2
#define NX 64
#define NY 64
#define DT 0.01
#define ITER 800000
#define LX 128*M_PI
#define LY 128*M_PI
#define discret "real"
#define scheme "eulis"

/* possible values for scheme :
 * "eule"
 * "euli"
 * "cn"
 * "rk4"
 * */

/* possible values for discret :
 * "fourier"
 * "real"
 * */
#endif

#ifdef __cplusplus
}
#endif 
