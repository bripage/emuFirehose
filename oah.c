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
	long i = hash, state = 0;

	long acquire = ATOMIC_CAS(&hash_table[i], id, -1);
	if (acquire == -1 || acquire == id){
		// insert and update state table
		state = ATOMIC_ADDM(&hash_state[i], 1);
        if (state % ALERT_THRESHOLD == 0) {
            printf("Alert @ %lu\n", id);
            fflush(stdout);
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
        state = ATOMIC_ADDM(&hash_state[i], 1);
        if (state % ALERT_THRESHOLD == 0) {
            printf("Alert @ %lu\n", id);
            fflush(stdout);
        }
	}
}
