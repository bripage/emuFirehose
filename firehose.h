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
replicated long * address_hits;
replicated long * payload_state;
replicated long* stats;
long * packet_index;
long file_packets;
long nodelets_used;

FILE * ifp;

#endif //EMUFIREHOSE_FIREHOSE_H
