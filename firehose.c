//
// Created by bpage1 on 1/24/20.
//
#include "firehose.h"
#include "utility.h"
#include "deluge.h"
#include "structdef.h"

noinline long main(int argc, char **argv) {
    printf("%d\n", argc);

    for (int i = 0; i < argc; i++){
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
    MIGRATE(&hash_table[0]);

	cilk_spawn
	printf("Calling recursive_spawn()\n");
    fflush(stdout);
	recursive_spawn(0, NODELETS());
	cilk_sync;
	printf("***Computation Complete***\n");
    fflush(stdout);



    cleanup();
    return 0;
}