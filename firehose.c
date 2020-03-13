//
// Created by bpage1 on 1/24/20.
//
#include "firehose.h"
#include "utility.h"
#include "deluge.h"
#include "structdef.h"

noinline long main(int argc, char **argv) {
    long i, j;
    for (i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }

    volatile uint64_t start_time, total_time;
    volatile double execution_time;
    parse_args(argc, argv);
    init();

    for (long thread_count = 1; thread_count <= 64; thread_count *= 2) {
        mw_replicated_init(&threads_per_nodelet, thread_count);
        printf("\nThreads Per Nodelet = %ld\n", threads_per_nodelet);
        fflush(stdout);

        MIGRATE(&payload_state[0]);

        start_time = CLOCK();
        cilk_spawn
        recursive_spawn(0, nodelets_used);
        cilk_sync;
        total_time = CLOCK() - start_time;
        printf("***Computation Complete***\n");
        fflush(stdout);


        for (i = 0; i < nodelets_used; i++) {
            cilk_migrate_hint(&payload_state[i]);
            cilk_spawn compile_hit_counts(i);
        }
        cilk_sync;

        long max_occurance = 0, next_addr, unique_keys = 0, temp, migrations = 0, dead_keys = 0;
        for (i = 0; i < 6400000; i++) {
            temp = payload_state[i];
            next_addr = temp >> 32;
            if (next_addr > max_occurance) {
                max_occurance = next_addr;

            }
            if (payload_state[i] != 0) {
                unique_keys++;
            }
            if (next_addr >= 24) {
                dead_keys++;
            }
        }

        cleanup1();
        cilk_spawn
        recursive_spawn_count(0, nodelets_used);
        cilk_sync;
        printf("***Migration Count Run Complete***\n");
        fflush(stdout);

        execution_time = (double) total_time / CLOCK_RATE;
        printf("Datums Received: %ld\n", numDatums*nodelets_used);
        printf("Unique Keys: %ld\n", unique_keys);
        printf("Max occurance of any key: %ld\n", max_occurance);
        printf("Event Count: %ld\n", stats[0]);
        printf("True Anomalies: %ld\n", stats[1]);
        printf("False Positives: %ld\n", stats[2]);
        printf("True Negatives: %ld\n", stats[3]);
        printf("False Negatives: %ld\n", stats[4]);
        printf("True Bias Flag Count: %ld\n", stats[5]);
        printf("Dead Keys: %ld\n", dead_keys);
        printf("Execution Time = %lf msec.\n", execution_time * 1000);
        printf("Datums per msec = %lf \n", (numDatums * nodelets_used) / (execution_time * 1000));
        fflush(stdout);

        printf("\nThread Migrations:\n");
        for (i = 0; i < nodelets_used; i++) {
            migrations = thread_migrations[i];
            printf("%ld: %ld\n", i, migrations);
        }
        fflush(stdout);

        cleanup();
    }

    return 0;
}