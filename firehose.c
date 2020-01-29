//
// Created by bpage1 on 1/24/20.
//
#include "firehose.h"
#include "utility.h"
#include "deluge.h"
#include "structdef.h"

noinline long main(int argc, char **argv) {
    printf("%d\n", argc);

    long i;
    for (i = 0; i < argc; i++){
        printf("%s\n",argv[i]);
    }

    volatile uint64_t start_time, total_time;
    volatile double execution_time;

    printf("Nodelets = %ld, Threads per nodelet = %ld\n", NODELETS(), THREADS_PER_NODELET);

    parse_args(argc, argv);

    printf("Initialization start.\n");
    init();
    printf("Initialization end.\n");

    printf("Calling MIGRATE().\n");
    fflush(stdout);
    MIGRATE(&address_hits[0]);

	cilk_spawn
	printf("Calling recursive_spawn()\n");
    fflush(stdout);
	recursive_spawn(0, NODELETS());
	cilk_sync;
	printf("***Computation Complete***\n");
    fflush(stdout);


    long max_occurance = 0, next_addr, unique_keys = 0;
    for (i = 0; i < 100000; i++){
        next_addr = address_hits[i];
        if (next_addr > max_occurance){
            max_occurance = next_addr;
        }

        if (hash_table[i] != -1){
            unique_keys++;
        }
    }
    printf("Datums Received: %ld\n", file_packets);
    printf("Unique Keys: %ld\n", 0);
    printf("Max occurance of any key: %ld\n", max_occurance);
    printf("Event Count: %ld\n", event_count);
    printf("True Anomalies: %ld\n", true_positive);
    printf("False Positives: %ld\n", false_positve);
    printf("True Negatives: %ld\n", true_negative);
    printf("False Negatives: %ld\n", false_negative);
    fflush(stdout);

    cleanup();
    return 0;
}