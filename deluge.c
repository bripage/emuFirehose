//
// Created by Brian Page on 2020-01-27.
//
#include "deluge.h"
#include "structdef.h"

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
		cilk_migrate_hint(&hash_table[mid]);
		cilk_spawn recursive_spawn(mid, high);

		high = mid;
	}

	for (i = 0; i < THREADS_PER_NODELET; i++) {
	    printf("thread %ld on nodelet %ld lanching spray()\n", i, nodelet);
        fflush(stdout);
		cilk_spawn spray(i, nodelet);
	}
}

void spray(long i, long n){
	unsigned long addr;
	long val, flag;
	struct packet * wdn = workload_dist[n];
	long local_list_end = dist_end;
    long hash, i, state = 0, acquire;

	while (i < local_list_end) {
		addr = wdn[i].address;
		val = wdn[i].val;
		flag = wdn[i].flag;

		//handle_packet(addr, val, flag);
        //unsigned long id = address;
        hash = addr % 100000; //inline this
        i = hash;

        acquire = ATOMIC_CAS(&hash_table[i], addr, -1);
        if (acquire == -1 || acquire == addr){
            // insert and update state table
            state = ATOMIC_ADDM(&hash_state[i], 1);
            if (state % 24 == 0) {
                printf("Alert @ %lu\n", addr);
                fflush(stdout);
            }
        } else {
            // slot take, find an empty one
            if (i+1 == 100000) {
                i = 0;
            } else {
                i++;
            }

            acquire = ATOMIC_CAS(&hash_table[i], addr, -1);
            while (acquire != -1 || acquire != addr){
                if (i == hash){
                    printf("ERROR: Hash table FULL\n");
                    fflush(stdout);
                    exit(1);
                }
                i++;
            }

            // now that we have either found the key in the hashtable or located an
            // empty slot, add or update the state for the given location and key
            state = ATOMIC_ADDM(&hash_state[i], 1);
            if (state % 24 == 0) {
                printf("Alert @ %lu\n", id);
                fflush(stdout);
            }
        }

		printf("%ld handled packet %ld\n", n , i);
        fflush(stdout);

		i+=THREADS_PER_NODELET;
	}
}