//
// Created by Brian Page on 2020-01-27.
//
#include "deluge.h"
#include "structdef.h"

void recursive_spawn(long low, long high) {
    long i;
    long nodelet = NODE_ID();
    // Want a grainsize of 1 to spawn a threadlet at each nodelet
    for (;;) {
        unsigned long count = high - low;
        // spawn a threadlet at each nodelet
        if (count <= 1) break;
        // Invariant: count >= 2
        unsigned long mid = low + count / 2;
        cilk_migrate_hint(&payload_state[mid]);
        cilk_spawn recursive_spawn(mid, high);
        high = mid;
    }

    for (i = 0; i < threads_per_nodelet; i++) {
        cilk_spawn spray(i, nodelet);
    }
}

void recursive_spawn_count(long low, long high) {
    long i;
    long nodelet = NODE_ID();
    // Want a grainsize of 1 to spawn a threadlet at each nodelet
    for (;;) {
        unsigned long count = high - low;
        // spawn a threadlet at each nodelet
        if (count <= 1) break;
        // Invariant: count >= 2
        unsigned long mid = low + count / 2;
        cilk_migrate_hint(&payload_state[mid]);
        cilk_spawn recursive_spawn_count(mid, high);
        high = mid;
    }

    for (i = 0; i < threads_per_nodelet; i++) {
        cilk_spawn spray_count(i, nodelet);
    }
}

void spray(long i, long n) {
    unsigned long addr;
    long val, flag;
    unsigned long hits = 0, payload = 0, state = 0, temp;
    struct packet *wdn = workload_dist[n];
    long local_list_end = dist_end;
    long hash, j;
    long hashSize = 6400000;

    while (i < local_list_end) {
        addr = wdn[i].address;
        val = wdn[i].val;
        flag = wdn[i].flag;

        i += threads_per_nodelet;

        if (flag == 1) {
            REMOTE_ADD(&stats[5], 1);
        }

        hash = addr % hashSize; //inline this
        j = hash;

        //not currenly dead
        state = payload_state[j];

        temp = 4294967296 + val;
        state = ATOMIC_ADDM(&payload_state[j], temp);
        hits = state >> 32;
        payload = state & 4294967295;

        if (hits == 24) {
            REMOTE_ADD(&stats[0], 1);
            if (payload > 4) { //too high to be anomaly
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
        }
    }
}

void spray_count(long i, long n){
    unsigned long addr;
    long val;
    unsigned long hits = 0, payload = 0, state = 0, temp;
    struct packet *wdn = workload_dist[n];
    long local_list_end = dist_end;
    long hash, j;
    long hashSize = 6400000;

    while (i < local_list_end) {
        addr = wdn[i].address;
        val = wdn[i].val;

        i += threads_per_nodelet;

        hash = addr % hashSize; //inline this
        j = hash;

        check = j % nodelets_used;
        if (check != n) {
            REMOTE_ADD(&thread_migrations[n], 1);
        }
    }
}