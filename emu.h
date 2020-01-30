// 
// Main header file for EMU
//

#include "memoryweb.h"
#include "malloc.h"
#include "distributed.h"
#include "cilk.h"
#include "timing.h"
#include "stdio.h"

#define DEBUG 0

#ifndef CLOCK_RATE
#define CLOCK_RATE 175000000
#endif

#ifndef THREADS_PER_NODELET
#define THREADS_PER_NODELET 2
#endif
