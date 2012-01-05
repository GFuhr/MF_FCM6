#include "../include/common.h"
#ifndef M_PI
#define M_PI 3.141592653589793
#endif

#define C 0.1
#define NX 256
#define NY 256
#define DT 0.001
#define ITER 100000
#define LX 128*M_PI
#define LY 128*M_PI
#define discret "fourier"
#define scheme "eule"

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
