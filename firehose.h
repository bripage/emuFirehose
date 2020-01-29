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
extern replicated long * address_hits;
extern replicated long * payload_state;
replicated long event_count;
replicated long true_positive;
replicated long false_positve;
replicated long true_negative;
replicated long false_negative;
long * packet_index;
long file_packets;
long event_count;
FILE * ifp;

#endif //EMUFIREHOSE_FIREHOSE_H
