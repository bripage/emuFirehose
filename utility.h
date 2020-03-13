//#ifndef EMUFIREHOSE_UTILITY_H
//#define EMUFIREHOSE_UTILITY_H

#include "emu.h"
#include "stdlib.h"
#include "unistd.h"
#include "stddef.h"
#include "fcntl.h"
#include <math.h>
#include "powerlaw.h"

extern replicated struct packet ** workload_dist;
extern replicated long PACKET_COUNT;
extern replicated long dist_end;
extern replicated long nodelet_count;
extern replicated long * payload_state;
extern replicated long * thread_migrations;
extern replicated long ** delegate_payload_state;
extern replicated long* stats;
extern long * packet_index;
extern FILE * ifp;
struct packet;
extern replicated long nodelets_used;
extern replicated long threads_per_nodelet;
extern replicated long numDatums;

void parse_args(int argc, char * argv[]);
void init();
void cleanup();
void cleanup1();
void compile_hit_counts(long n);


//#endif //EMUFIREHOSE_UTILITY_H