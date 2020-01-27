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

replicated struct element ** workload_dist;
replicated long PACKET_COUNT;
replicated long dist_end;
replicated long nodelet_count;
replicated long * hash_table;
replicated long * hash_table_state;
long * packet_index;
long file_packets;


#endif //EMUFIREHOSE_FIREHOSE_H
