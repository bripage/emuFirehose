//#ifndef EMUFIREHOSE_UTILITY_H
//#define EMUFIREHOSE_UTILITY_H

#include "emu.h"
#include "stdlib.h"
#include "unistd.h"
#include "stddef.h"
#include "fcntl.h"
#include "oah.h"
#include <math.h>

extern replicated struct packet ** workload_dist;
extern replicated long PACKET_COUNT;
extern replicated long dist_end;
extern replicated long nodelet_count;
extern replicated long * hash_table;
extern replicated long * hash_state;
extern replicated long * hash_state2;
extern long * packet_index;
extern long file_packets;
extern FILE * ifp;
struct packet;

void parse_args(int argc, char * argv[]);
void init();
void cleanup();

//#endif //EMUFIREHOSE_UTILITY_H