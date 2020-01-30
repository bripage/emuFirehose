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
		cilk_spawn spray(i, nodelet);
	}
}

void spray(long i, long n){
	unsigned long addr, acquire;
	long val, flag;
	struct packet * wdn = workload_dist[n];
	long local_list_end = dist_end;
    long hash, j, hits = 0, payload = 0, swap_state, state = 0, temp = 0;

	while (i < local_list_end) {
		addr = wdn[i].address;
		val = wdn[i].val;
		flag = wdn[i].flag;

		if (flag == 1){
		    REMOTE_ADD(&stats[5], 1);
		}

        hash = addr % 100000; //inline this
        j = hash;
        acquire = ATOMIC_CAS(&hash_table[j], addr, -1);
        if (acquire == -1 || acquire == addr){  // found an empty slot on the first try (woohoo)
            // insert and update state table
            state = ATOMIC_ADDM(&payload_state[j], 4294967297); // increment both high 32 and low bits by one.
            temp = state;
            //printf("state = %ld\n",state);
            //fflush(stdout);
            //hits = ATOMIC_ADDM(&(address_hits[j]), 1);
            //payload = ATOMIC_ADDM(&payload_state[j], 1);
            hits = temp & 4294967295;
            payload = state >> 32;
            //printf("state = %ld, hits = %ld, payload = %ld\n",state, hits, payload);
            //fflush(stdout);
            if (hits % 24 == 0 && hits != 0) {
                REMOTE_ADD(&stats[0], 1);
                ATOMIC_SWAP(&(payload_state[j]), 0);
                if (payload <= 4 && flag == 1){
                    REMOTE_ADD(&stats[1], 1);
                } else if (payload <= 4 && flag == 0){
                    REMOTE_ADD(&stats[2], 1);
                } else if (payload > 4 && flag == 1){
                    REMOTE_ADD(&stats[4], 1);
                } else if (payload > 4 && flag == 0){
                    REMOTE_ADD(&stats[3], 1);
                }
            }

        } else {    // slot taken, find an empty one
            if (j+1 == 100000) {
                j = 0;
            } else {
                j++;
            }

            acquire = ATOMIC_CAS(&hash_table[j], addr, -1);
            while (acquire != -1 || acquire != addr){
                if (i == hash){
                    printf("ERROR: Hash table FULL\n");
                    fflush(stdout);
                    exit(1);
                }
                j++;
            }

            // now that we have either found the key in the hashtable or located an
            // empty slot, add or update the state for the given location and key
            //hits = ATOMIC_ADDM(&address_hits[j], 1);
            state = ATOMIC_ADDM(&payload_state[j], 4294967297); // increment both high 32 and low bits by one.
            temp = state;
            //printf("state = %ld\n",state);
            //fflush(stdout);
            //hits = ATOMIC_ADDM(&(address_hits[j]), 1);
            //payload = ATOMIC_ADDM(&payload_state[j], 1);
            hits = temp & 4294967295;
            payload = state >> 32;
            //printf("state = %ld, hits = %ld, payload = %ld\n",state, hits, payload);
            //fflush(stdout);
            if (hits % 24 == 0) {
                REMOTE_ADD(&stats[0], 1);
                ATOMIC_SWAP(&(payload_state[j]), 0);
                if (payload <= 4 && flag == 1){
                    REMOTE_ADD(&stats[1], 1);
                } else if (payload <= 4 && flag == 0){
                    REMOTE_ADD(&stats[2], 1);
                } else if (payload > 4 && flag == 0) {
                    REMOTE_ADD(&stats[3], 1);
                } else if (payload > 4 && flag == 1){
                    REMOTE_ADD(&stats[4], 1);
                }
            }
        }

		//printf("%ld handled packet %ld\n", n , i);
        //fflush(stdout);

		i+=THREADS_PER_NODELET;
	}

}

