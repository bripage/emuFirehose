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
#include "oah.h"
#include <math.h>

extern replicated struct packet ** workload_dist;
extern replicated long dist_end;
extern replicated long nodelet_count;
extern replicated unsigned long * hash_table;
extern replicated long * hash_state;
extern replicated long * hash_state2;
extern long * packet_index;
struct packet;

void spray(long i, long n);
void recursive_spawn(long low, long high);

#endif //EMUFIREHOSE_DELUGE_H
