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
        hits = ATOMIC_ADDM(&address_hits[ji], 1);
        if (hits % 24 == 0) {
            REMOTE_ADD(&event_count, 1);
            payload = ATOMIC_ADDM(&payload_state[i], val);
            ATOMIC_SWAP(payload_state[j], 0);
            if (payload <= 5 && flag == 1) {
                REMOTE_ADD(&true_positive, 1);
            } else if (payload <= 5 && flag == 0) {
                REMOTE_ADD(&false_positve, 1);
            } else if (payload > 5 && flag == 1) {
                REMOTE_ADD(&false_negative, 1);
            } else if (payload > 5 && flag == 0) {
                REMOTE_ADD(&true_negative, 1);
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
        hits = ATOMIC_ADDM(&address_hits[j], 1);
        if (hits % 24 == 0) {
            REMOTE_ADD(&event_count, 1);
            payload = ATOMIC_ADDM(&payload_state[j], val);
            ATOMIC_SWAP(payload_state[j], 0);
            if (payload <= 5 && flag == 1) {
                REMOTE_ADD(&true_positive, 1);
            } else if (payload <= 5 && flag == 0) {
                REMOTE_ADD(&false_positve, 1);
            } else if (payload > 5 && flag == 1) {
                REMOTE_ADD(&false_negative, 1);
            } else if (payload > 5 && flag == 0) {
                REMOTE_ADD(&true_negative, 1);
            }
        }
	}
}
