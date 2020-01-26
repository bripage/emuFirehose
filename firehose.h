//
// Created by bpage1 on 1/24/20.
//

#ifndef EMUFIREHOSE_FIREHOSE_H
#define EMUFIREHOSE_FIREHOSE_H
#include "emu.h"
#include "fcntl.h"
#include "time.h"
#include "stdlib.h"
#include "utility.h"

replicated struct element ** workload_dist;
replicated long PACKET_COUNT;
replicated long local_dist_end;
replicated long nodelet_count;
long binary_input;
long * packet_index;


#endif //EMUFIREHOSE_FIREHOSE_H
