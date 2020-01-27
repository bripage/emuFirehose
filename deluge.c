//
// Created by Brian Page on 2020-01-27.
//
#include "deluge.h"

void recursive_spawn(long low, long high){
	long i;
	long nodelet = NODE_ID();
	// Want a grainsize of 1 to spawn a threadlet at each nodelet
	for (;;) {
		unsigned long count = high - low;
		// spawn a threadlet at each nodelet
		if (count <= 1) break;
		// Invariant: count >= 2
		unsigned long mid = low + count / 2;
		cilk_migrate_hint(&result[mid]);
		cilk_spawn recursive_spawn(mid, high);

		high = mid;
	}

	for (i = 0; i < THREADS_PER_NODELET; i++) {
		cilk_spawn spray(i, nodelet);
	}
}

void spray(long i, long n){
	long addr, val, flag;
	struct packet * wln = workload_list[n];
	long local_list_end = dist_end;

	while (i < local_list_end) {
		addr = wln[i].address;
		val = wln[i].val;
		flag = wln[i].flag;

		handle_packet(addr, val, flag);

		i+=THREADS_PER_NODELET;
	}
}