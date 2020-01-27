#ifndef EMUFIREHOSE_UTILITY_H
#define EMUFIREHOSE_UTILITY_H

#include "emu.h"
#include "stdlib.h"
#include "unistd.h"
#include "stddef.h"
#include "fcntl.h"
#include "oah.h"
#include <math.h>

// Structure to hold non zero elements in workload list
struct packet {
	unsigned long address;
	long val;
	long flag;
};

extern replicated struct packet ** workload_dist;
extern replicated long PACKET_COUNT;
extern replicated long dist_end;
extern replicated long nodelet_count;
extern replicated long * hash_table;
extern replicated long * hash_table_state;
extern long * packet_index;
extern long file_packets;
extern FILE * ifp;

void parse_args(int argc, char * argv[]);
void init();
void cleanup();

#endif //EMUFIREHOSE_UTILITY_H