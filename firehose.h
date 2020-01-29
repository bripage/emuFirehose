//
// Created by bpage1 on 1/24/20.
//

#ifndef EMUFIREHOSE_FIREHOSE_H
#define EMUFIREHOSE_FIREHOSE_H
#include "emu.h"
#include "fcntl.h"
#include "time.h"
#include "stdlib.h"
#include "stdint.h"
#include "structdef.h"

replicated struct packet ** workload_dist;
replicated long PACKET_COUNT;
replicated long dist_end;
replicated long nodelet_count;
replicated unsigned long * hash_table;
replicated long * hash_state;
replicated long * hash_state2;
long * packet_index;
long file_packets;
FILE * ifp;
unsigned long * alarm_queue;




#endif //EMUFIREHOSE_FIREHOSE_H
