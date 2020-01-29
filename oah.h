//
// Created by Brian Page on 2020-01-26.
//
#ifndef EMUFIREHOSE_OAH_H
#define EMUFIREHOSE_OAH_H

#include "emu.h"
#include "stdlib.h"
#include "unistd.h"
#include "stddef.h"
#include "fcntl.h"
#include "oah.h"
#include <math.h>

extern replicated unsigned long * hash_table;
extern replicated long * address_hits;
extern replicated long * payload_state;
extern replicated long event_count;
extern replicated long true_positive;
extern replicated long false_positve;
extern replicated long true_negative;
extern replicated long false_negative;
struct packet;

void handle_packet(unsigned long address, long val, long flag);
void alert_check(long i, unsigned long id);

#endif //EMUFIREHOSE_OAH_H