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
#include "oah.h"
#include <math.h>

extern replicated struct packet ** workload_dist;
extern replicated long dist_end;
extern replicated long nodelet_count;
extern replicated unsigned long * hash_table;
extern replicated long * address_hits;
extern replicated long * payload_state;
extern replicated long* event_count;
extern replicated long* true_positive;
extern replicated long* false_positve;
extern replicated long* true_negative;
extern replicated long* false_negative;
extern long * packet_index;
struct packet;

void recursive_spawn(long low, long high);
void spray(long i, long n);

#endif //EMUFIREHOSE_DELUGE_H
