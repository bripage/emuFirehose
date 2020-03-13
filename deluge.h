//
// Created by Brian Page on 2020-01-27.
//
#ifndef EMUFIREHOSE_DELUGE_H
#define EMUFIREHOSE_DELUGE_H

#include "emu.h"
#include "stdlib.h"
#include "unistd.h"
#include "stddef.h"
#include "fcntl.h"
#include "time.h"
#include "stdint.h"
#include <math.h>

extern replicated struct packet ** workload_dist;
extern replicated long dist_end;
extern replicated long nodelet_count;
extern replicated long * payload_state;
extern replicated long * thread_migrations;
extern replicated long ** delegate_payload_state;
extern replicated long* stats;
extern long * packet_index;
struct packet;
extern long nodelets_used;
extern replicated long threads_per_nodelet;

void recursive_spawn(long low, long high);
void spray(long i, long n);
void spray_count(long i, long n);

#endif //EMUFIREHOSE_DELUGE_H
