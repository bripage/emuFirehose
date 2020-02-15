//#ifndef EMUFIREHOSE_UTILITY_H
//#define EMUFIREHOSE_UTILITY_H

#include "emu.h"
#include "stdlib.h"
#include "unistd.h"
#include "stddef.h"
#include "fcntl.h"
#include <math.h>

extern replicated struct packet ** workload_dist;
extern replicated long PACKET_COUNT;
extern replicated long dist_end;
extern replicated long nodelet_count;
extern replicated unsigned long * hash_table;
extern replicated long * payload_state;
extern replicated long* stats;
extern long * packet_index;
extern long file_packets;
extern FILE * ifp;
struct packet;
extern replicated long nodelets_used;
extern replicated long threads_per_nodelet;
extern replicated long* numPackets;
extern replicated long* datumsPerPacket;

void parse_args(int argc, char * argv[]);
void init();
void cleanup();


//#endif //EMUFIREHOSE_UTILITY_H