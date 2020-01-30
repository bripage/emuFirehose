//
// Created by Brian Page on 2020-01-26.
//
#include "oah.h"
#include "structdef.h"

#define HASHTABLESIZE 100000
#define ALERT_THRESHOLD 24

void handle_packet(unsigned long address, long val, long flag) {
	unsigned long id = address;
	long hash = id % HASHTABLESIZE; //inline this
	long i = hash, hits = 0, payload;

	long acquire = ATOMIC_CAS(&hash_table[i], id, -1);
	if (acquire == -1 || acquire == id){
		// insert and update state table
        hits = ATOMIC_ADDM(&address_hits[i], 1);
        if (hits % 24 == 0) {
            printf("inside 1\n");
            fflush(stdout);
            REMOTE_ADD(&stats[0], 1);
            printf("event_count = %ld\n", stats[0]);
            fflush(stdout);
            ATOMIC_SWAP(&(payload_state[j]), 0);
            printf("payload = %ld\n", payload);
            fflush(stdout);
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
	} else {
		// slot take, find an empty one
		if (i+1 == HASHTABLESIZE) {
			i = 0;
		} else {
			i++;
		}

        acquire = ATOMIC_CAS(&hash_table[i], id, -1);
		while (acquire != -1 || acquire != id){
			if (i == hash){
				printf("ERROR: Hash table FULL\n");
                fflush(stdout);
				exit(1);
			}
			i++;
		}

		// now that we have either found the key in the hashtable or located an
		// empty slot, add or update the state for the given location and key
        hits = ATOMIC_ADDM(&address_hits[i], 1);
        if (hits % 24 == 0) {
            printf("inside 1\n");
            fflush(stdout);
            REMOTE_ADD(&stats[0], 1);
            printf("event_count = %ld\n", stats[0]);
            fflush(stdout);
            ATOMIC_SWAP(&(payload_state[j]), 0);
            printf("payload = %ld\n", payload);
            fflush(stdout);
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
	}
}
