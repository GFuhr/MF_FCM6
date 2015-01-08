#ifdef _HANDCLOCK_
#define __USE_BSD

#ifdef __linux__
#include <sys/time.h>
#else
#include <time.h>
#endif
#include <string.h>
#include <stdlib.h>

#define mytimeradd(a, b, result)					\
  do {									\
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;			\
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;			\
    if ((result)->tv_usec >= 1000000)					\
      {									\
	++(result)->tv_sec;						\
	(result)->tv_usec -= 1000000;					\
      }									\
  } while (0)

#define mytimersub(a, b, result)					\
  do {									\
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;			\
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;			\
    if ((result)->tv_usec < 0) {					\
      --(result)->tv_sec;						\
      (result)->tv_usec += 1000000;					\
    }									\
  } while (0)

#define _MAXLEVEL 4
#define _MAXTIMER 100
#define _LABSIZE  40

typedef struct {
  struct timeval begin;
  struct timeval end;
  struct timeval ldiff;
  struct timeval gdiff;
  struct timeval tmp;
  int    level;
  float  ldiff_val, gdiff_val;
  char   label[_LABSIZE];
} TIMERSTRUCT;


#ifdef _CLOCK_VARDEFINE
int nbtimer[_MAXLEVEL] = {0};
TIMERSTRUCT* timer_list[_MAXTIMER][_MAXLEVEL];
#else
extern int nbtimer[_MAXLEVEL];
extern TIMERSTRUCT* timer_list[_MAXTIMER][_MAXLEVEL];
#endif

#define TIMER_DEFINE(var,mylabel,mylevel) {				\
    if (strncmp(mylabel,var.label,_LABSIZE)) {					\
      strncpy(var.label,mylabel,_LABSIZE);					\
      var.level = mylevel;						\
      var.ldiff.tv_sec=0; var.ldiff.tv_usec=0;				\
      var.gdiff.tv_sec=0; var.gdiff.tv_usec=0;				\
      if (nbtimer[mylevel] == _MAXTIMER-1) { printf("Error in clock.h, nbtimer too high\n"); exit(1); } \
      timer_list[(nbtimer[mylevel])++][mylevel] = &var;			\
    }}
#define TIMER_BEGIN(var) {gettimeofday(&var.begin, NULL);}
#define TIMER_END(var)   {	\
  gettimeofday(&var.end, NULL); \
  mytimersub(&var.end, &var.begin, &var.ldiff); \
  var.tmp=var.gdiff; \
  mytimeradd(&var.tmp,&var.ldiff,&var.gdiff); \
  var.ldiff_val = var.ldiff.tv_sec*1000000.+var.ldiff.tv_usec; \
  var.gdiff_val = var.gdiff.tv_sec*1000000.+var.gdiff.tv_usec; \
  } 
#define TIMER_LDIFF(var,fdiff) fdiff = var.ldiff_val
#define TIMER_GDIFF(var,fdiff) fdiff = var.gdiff_val

#define TIMER_PRINT_LIST(mylevel) { int _i; double _fsum;		\
    printf("== begin timer list at level %d ==\n",mylevel);		\
    for (_i=0, _fsum=0.; _i<nbtimer[mylevel]; _i++) {			\
      printf("Timer %40s lastcall : %23.5f us  summing up all calls: %23.5f us\n", \
	     (timer_list[_i][mylevel])->label,				\
	     (timer_list[_i][mylevel])->ldiff_val,			\
	     (timer_list[_i][mylevel])->gdiff_val);			\
      _fsum += (timer_list[_i][mylevel])->gdiff_val;			\
    }									\
    printf("== end timer list ==, sum of all calls: %25.5f\n",_fsum);	\
  }

#else

typedef void* TIMERSTRUCT;

#define TIMER_DEFINE(var,mylabel,mylevel)
#define TIMER_BEGIN(var)
#define TIMER_END(var)
#define TIMER_LDIFF(var,fdiff) 
#define TIMER_GDIFF(var,fdiff) 
#define TIMER_PRINT_LIST(mylevel)

#endif

#ifdef _TAU_ 
#include <TAU.h>
#else

#define TAU_PROFILE_INIT(argc, argv)
#define TAU_PROFILE_SET_NODE(arg)
#define TAU_PROFILE_TIMER(var, label1, label2, group)
#define TAU_PROFILE_START(var)
#define TAU_PROFILE_STOP(var)

#endif 

