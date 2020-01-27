//
// Created by Brian Page on 2020-01-26.
//
#include "oah.h"
#include "structdef.h"

#define HASHTABLESIZE 100000
#define alert_threshold 16

void handle_packet(unsigned long address, long val, long flag) {
	unsigned long id = address;
	long hash = id % HASHTABLESIZE;
	long i = hash;

	if (hash_table[i] == -1 || hash_table[i] == id){
		// insert and update state table
		hash_table[i] = id;
		hash_table_state[i]++;
		alert_check(i);
	} else {
		// slot take, find an empty one
		if (i+1 == HASHTABLESIZE) {
			i = 0;
		} else {
			i++;
		}
		while (hash_table[i] != id && hash_table[i] != -1){
			if (i == hash){
				printf("ERROR: Hash table FULL\n");
				exit(1);
			}
			i++;
		}

		// now that we have either found the key in the hashtable or located an
		// empty slot, add or update the state for the given location and key
		hash_table[i] = id;
		hash_table_state[i]++;
		alert_check(i, id);
	}
}

void alert_check(long i, unsigned long id){
	if (hash_table_state[i] == alert_threshold) {
		printf("Alert @ %ul\n", id);
		hash_table_state[i] = 0;
	}
}
