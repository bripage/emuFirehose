//
// Created by bpage1 on 1/24/20.
//
#indlude "utility.h"
#include "firehose.h"
#include "stdint.h"

noinline long main(int argc, char **argv) {
    printf("%d\n", argc);

    for (int i = 0; i < argc; i++){
        printf("%s\n",argv[i]);
    }
    struct args args;
    volatile uint64_t start_time, total_time;
    volatile double execution_time;

    printf("Nodelets = %ld, Threads per nodelet = %ld\n", NODELETS(), THREADS_PER_NODELET);

    parse_args(&spmv_args, argc, argv);

    printf("Initialization start.\n");
    init(&spmv_args);
    printf("Initialization end.\n");

    printf("Calling MIGRATE().\n");
    fflush(stdout);

    MIGRATE(&X[0]);



    cleanup();

    return 0;
}