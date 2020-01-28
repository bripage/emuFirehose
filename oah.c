//
// Created by Brian Page on 2020-01-26.
//
#include "oah.h"
#include "structdef.h"

#define HASHTABLESIZE 100000
#define alert_threshold 24

void handle_packet(unsigned long address, long val, long flag) {
	unsigned long id = address;
	long hash = id % HASHTABLESIZE;
	long i = hash, state = 0;

	long acquire = ATOMIC_CAS(&hash_table[i], id, -1);
	if (acquire == -1 || acquire == id){
		// insert and update state table
		state = ATOMIC_ADDM(&hash_state[i], 1);
		alert_check(state, id);
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
				exit(1);
			}
			i++;
		}

		// now that we have either found the key in the hashtable or located an
		// empty slot, add or update the state for the given location and key
        state = ATOMIC_ADDM(&hash_state[i], 1);
		alert_check(state, id);
	}
}

void alert_check(long state, unsigned long id){
	if (state % alert_threshold == 0) {
		printf("Alert @ %ul\n", id);
	}
}
