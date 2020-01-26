#include "emu.h"
#include "stdlib.h"
#include "unistd.h"
#include "stddef.h"
#include "fcntl.h"
#include <math.h>

// Structure to hold non zero elements in workload list
struct packet {
	unsigned long id;
	char val;
	char flag;
};

// Structure to hold program arguments
struct args {
	long PACKET_COUNT;
};

extern replicated struct packet ** workload_dist;
extern replicated long PACKET_COUNT;
extern replicated long local_dist_end;
extern replicated long nodelet_count;
extern long binary_input;
extern long * packet_index;
struct args;
extern FILE * ifp;

void parse_args(struct args * args, int argc, char * argv[]);
void init(struct * args);
void cleanup();