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

	aq_index = 0; // make sure alarms are put into the alarm_queue starting at element 0
	comp_done = 0; // when this = THREADS_PER_NODELET, alarm output stops on local nodelet

	for (i = 0; i < THREADS_PER_NODELET+1; i++) {
		if (i == THREADS_PER_NODELET) {
			// handle alert printfs with this single thread
			printf("thread %ld on nodelet %ld lanching alarm_control()\n", i, nodelet);
			fflush(stdout);
			cilk_spawn alarm_control(i, nodelet);
		} else {
			printf("thread %ld on nodelet %ld lanching spray()\n", i, nodelet);
			fflush(stdout);
			cilk_spawn spray(i, nodelet);
		}
	}
}

void spray(long i, long n){
	unsigned long addr, acquire;
	long val, flag;
	struct packet * wdn = workload_dist[n];
	long local_list_end = dist_end;
    long hash, j, state = 0;
	long queue_i, queue_slot, acquire_aq;

	while (i < local_list_end) {
		addr = wdn[i].address;
		val = wdn[i].val;
		flag = wdn[i].flag;

		//handle_packet(addr, val, flag);
        //unsigned long id = address;
        hash = addr % 100000; //inline this
        j = hash;

        acquire = ATOMIC_CAS(&hash_table[j], addr, -1);
        if (acquire == -1 || acquire == addr){  // found an empty slot on the first try (woohoo)
            // insert and update state table
            state = ATOMIC_ADDM(&hash_state[j], 1);
            if (state % 24 == 0) {
                //printf("Alert @ %lu\n", addr);
                //fflush(stdout);
	            do {
		            queue_i = ATOMIC_ADDM(&aq_index, 1);
		            queue_slot = queue_i % 1000;
		            acquire_aq = ATOMIC_CAS(&alarm_queue[queue_slot], addr, 0);
	            } while (acquire_aq != 0); // if the queue slot is taken go around until you find one!
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
            state = ATOMIC_ADDM(&hash_state[j], 1);
            if (state % 24 == 0) {
                //printf("Alert @ %lu\n", addr);
                //fflush(stdout);
	            do {
		            queue_i = ATOMIC_ADDM(&aq_index, 1);
		            queue_slot = queue_i % 1000;
		            acquire_aq = ATOMIC_CAS(&alarm_queue[queue_slot], addr, 0);
	            } while (acquire_aq != 0); // if the queue slot is taken go around until you find one!
            }
        }

		//printf("%ld handled packet %ld\n", n , i);
        fflush(stdout);

		i+=THREADS_PER_NODELET;
	}
	comp_done++;
}

void alarm_control(long i, long n){
	long j, addr;

	while (comp_done != THREADS_PER_NODELET){
		for (j = 0; j < 1000; j++){
			addr = alarm_queue[j];
			if (addr != 0){ // alarm exists at this location, send it!
				printf("Alert @ %lu\n", addr);
				fflush(stdout);
				REMOTE_ADD(&alarm_queue[j], -1*addr);
			}
		}
	}
}

void trigger_alarm(unsigned long addr){
	long queue_i, queue_slot, acquire;
	do {
		queue_i = ATOMIC_ADDM(&aq_index, 1);
		queue_slot = queue_i % 1000;
		acquire = ATOMIC_CAS(&alarm_queue[queue_slot], addr, 0);
	} while (acquire != 0); // if the queue slot is taken go around until you find one!
}
