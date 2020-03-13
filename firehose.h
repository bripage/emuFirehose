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
replicated long * payload_state;
replicated long * thread_migrations;
replicated long ** delegate_payload_state;
replicated long* stats;
replicated long numDatums;
long * packet_index;
replicated long nodelets_used;
replicated long threads_per_nodelet;

FILE * ifp;

#endif //EMUFIREHOSE_FIREHOSE_H
