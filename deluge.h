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
extern replicated long * hash_state;
extern replicated long * hash_state2;
extern long * packet_index;
struct packet;
extern unsigned long * alarm_queue;
long aq_index;
long comp_done;

void spray(long i, long n);
void recursive_spawn(long low, long high);
void alarm_control(long i, long n);
void trigger_alarm(unsigned long addr);

#endif //EMUFIREHOSE_DELUGE_H
