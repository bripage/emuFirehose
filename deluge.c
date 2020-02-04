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

	for (i = 0; i < threads_per_nodelet; i++) {
		cilk_spawn spray(i, nodelet);
	}
}

void spray(long i, long n){
	unsigned long addr, acquire;
	long val, flag;
	struct packet * wdn = workload_dist[n];
	long local_list_end = dist_end;
    long hash, j, k, l, hits = 0, payload = 0, swap_state, state = 0, temp = 0;
    //long hit_threshold = 24 * nodelets_used;
	//long payload_threshold = 4 * nodelets_used;
	long hit_threshold = 24;
	long payload_threshold = 4;

    //printf("hit_threshold = %ld, payload_hreashold = %ld\n", hit_threshold, payload_threshold);
    //fflush(stdout);

    while (i < local_list_end) {
        if (i % 100000 == 0) {
            printf("@ Datum #%ld\n", i);
            fflush(stdout);
        }

        addr = wdn[i].address;
        val = wdn[i].val;
        flag = wdn[i].flag;

        i += threads_per_nodelet;

        if (flag == 1) {
            REMOTE_ADD(&stats[5], 1);
        }

        hash = addr % 100000; //inline this
        j = hash;
        acquire = ATOMIC_CAS(&hash_table[j], addr, -1);
        // insert and update state table
        state = payload_state[j];
        //hits = state & 4294967295;
        payload = state >> 32;
        if (payload < 0) {
            ATOMIC_ADDM(&payload_state[j], 1);
            continue;
        }
        state = ATOMIC_ADDM(&payload_state[j], (4294967296 * val) + 1);
        hits = state & 4294967295;
        payload = state >> 32;
        if (hits == hit_threshold) {
            REMOTE_ADD(&stats[0], 1);
            if (payload > payload_threshold) { //too high to be anomaly
                if (flag) { // if true then it SHOULD have been one
                    REMOTE_ADD(&stats[4], 1);
                } else { // false means it wasnt supposed to be one! yay!
                    REMOTE_ADD(&stats[3], 1);
                }
            } else { // anomaly present
                if (flag) { // if true its a real one
                    REMOTE_ADD(&stats[1], 1);
                } else { // wasnt supposed to be one
                    REMOTE_ADD(&stats[2], 1);
                }
            }
            state = ATOMIC_ADDM(&payload_state[j], -4294967296);
        }
    }
}

