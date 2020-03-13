//
// Created by bpage1 on 1/24/20.
//
#include "utility.h"
#include "structdef.h"

void parse_args(int argc, char * argv[]) {
    long nu = atoi(argv[1]);
    mw_replicated_init(&nodelets_used, nu);
    printf("Nodelets used = %ld\n", nodelets_used);
    fflush(stdout);

    long np = atoi(argv[2]);
    mw_replicated_init(&numDatums, np);
    printf("Datum Count = %ld\n", numDatums);
    fflush(stdout);
}

long init_dist_end(long nodelet) {
    return packet_index[nodelet];
}

void get_data_and_distribute() {
    cilk_spawn generateDatums(0);
    mw_replicated_init(&dist_end, numDatums);
}

void compile_local_data(long n) {
    long i;
    long migrations = 0;
    long * delegate_state = delegate_payload_state[n];

    for (i = 0; i < 6400000; i++) {
        if (delegate_state[i] > 0) {
            migrations += delegate_state[i] - 1;
        }
    }
    thread_migrations[n] = migrations;
}

void init_mem(long n){
    long i;
    struct packet * wdn = workload_dist[n];
    long * dps = delegate_payload_state[n];

    for (i = 0; i < numDatums; i++){
        wdn[i].address = 0;
        wdn[i].val = 0;
        wdn[i].flag = 0;
    }

    for (i = 0; i < 6400000; i++){
        dps[i] = 0;
    }
}

void clear_mem(long n){
    long i;
    long * dps = delegate_payload_state[n];

    for (i = 0; i < 6400000; i++){
        dps[i] = 0;
    }
}

void compile_hit_counts(long n) {
    long i, count;
    long * delegate_state = delegate_payload_state[n];

    for (i = 0; i < 6400000; i++) {
        if (delegate_state[i] > 0) {
            count = delegate_state[i] - 1;
            REMOTE_ADD(&payload_state[i], count);
        }
    }
}

void init(long tph){
    long i, j;
    long nc = NODELETS();
    printf("Replicated init start.\n");
    fflush(stdout);
    mw_replicated_init(&nodelet_count, nc);

    long * s = (long *) mw_malloc1dlong(nodelet_count);
    printf("s allocated\n");
    fflush(stdout);
    if (s == NULL) {
        printf("Cannot allocate memory for stats array.\n");
        exit(1);
    }
    printf("checked s.\n");
    fflush(stdout);
    mw_replicated_init(&stats, s);
    for (i = 0; i < nodelet_count; i++){
        stats[i] = 0;
    }

    long * ps = (long *) mw_malloc1dlong(6400000);
    printf("ps allocated\n");
    fflush(stdout);
    if (ps == NULL) {
        printf("Cannot allocate memory for global payload state.\n");
        exit(1);
    }
    printf("checked gps.\n");
    fflush(stdout);
    mw_replicated_init(&payload_state, ps);
    for (i = 0; i < 6400000; i++){
        payload_state[i] = 0;
    }

    long * ma = (long *) mw_malloc1dlong(nodelet_count);
    printf("ma allocated\n");
    fflush(stdout);
    if (ma == NULL) {
        printf("Cannot allocate memory for migrations avoided.\n");
        exit(1);
    }
    printf("checked ma.\n");
    fflush(stdout);
    mw_replicated_init(&thread_migrations, ma);
    for (i = 0; i < nodelet_count; i++){
        thread_migrations[i] = 0;
    }

    long ** dps = (long **) mw_malloc2d(nodelet_count, 6400000 * sizeof(long));
    printf("dps allocated\n");
    fflush(stdout);
    if (dps == NULL) {
        printf("Cannot allocate memory for hash table.\n");
        exit(1);
    }
    printf("checked dps.\n");
    fflush(stdout);
    mw_replicated_init(&delegate_payload_state, dps);

    struct element ** wd;
    long packets_per_nodelet;
    //packets_per_nodelet = ceil(numDatums/nodelets_used);
    packets_per_nodelet = numDatums;
    printf("packets_per_nodelet = %ld\n", packets_per_nodelet);
    fflush(stdout);
    wd = (struct packet **) mw_malloc2d(nodelet_count, packets_per_nodelet * sizeof(struct packet));
    if (wd == NULL) {
        printf("Cannot allocate memory for workload_dist.\n");
        exit(1);
    }
    mw_replicated_init(&workload_dist, wd);
    printf("workload_dist replicated\n");
    fflush(stdout);

    for (i = 0; i < nodelets_used; i++) {
        cilk_migrate_hint(&payload_state[i]);
        cilk_spawn init_mem(i);
    }
    cilk_sync;
    printf("Done zeroing out packet memory\n");
    fflush(stdout);

    packet_index = (long *) malloc(nodelet_count * sizeof(long));
    for (i = 0; i < nodelet_count; i++){
        packet_index[i] = 0;
    }

    //get_data_and_distribute();
    cilk_spawn generateDatums(0);
    mw_replicated_init(&dist_end, packets_per_nodelet);
}

void cleanup1() {
    long i;

    for (i = 0; i < nodelets_used; i++) {
        cilk_migrate_hint(&payload_state[i]);
        cilk_spawn clear_mem(i);
    }
    cilk_sync;

    for (i = 0; i < 6400000; i++){
        payload_state[i] = 0;
    }
}

void cleanup() {
    long i;

    for (i = 0; i < nodelets_used; i++) {
        cilk_migrate_hint(&payload_state[i]);
        cilk_spawn clear_mem(i);
    }
    cilk_sync;

    for (i = 0; i < 6400000; i++){
        payload_state[i] = 0;
    }

    for (i = 0; i < nodelet_count; i++) {
        stats[i] = 0;
        thread_migrations[i] = 0;
    }
}