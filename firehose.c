//
// Created by bpage1 on 1/24/20.
//
#include "firehose.h"
#include "utility.h"
#include "deluge.h"
#include "structdef.h"

noinline long main(int argc, char **argv) {
	//printf("%d\n", argc);

	long i;
	for (i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}

	volatile uint64_t start_time, total_time;
	volatile double execution_time;

	//printf("Nodelets = %ld, Threads per nodelet = %ld\n", NODELETS(), THREADS_PER_NODELET);

	parse_args(argc, argv);

	//printf("Initialization start.\n");
	init();
	//printf("Initialization end.\n");

	//printf("Calling MIGRATE().\n");
	//fflush(stdout);
	//MIGRATE(&address_hits[0]);

	for (long n_count = 1; n_count <= 8; n_count++) {
		printf("\n\n*************** %ld Nodelets ***************\n", n_count);
		fflush(stdout);
        mw_replicated_init(&nodelets_used, n_count);

		for (long thread_count = 1; thread_count <= 64; thread_count *= 2) {
			MIGRATE(&payload_state[0]);
			mw_replicated_init(&threads_per_nodelet, thread_count);
			printf("\nThreads Per Nodelet = %ld\n", threads_per_nodelet);
			fflush(stdout);

			//if (n_count == 1) {
			//	start_time = CLOCK();
            //    for (i = 0; i < threads_per_nodelet; i++) {
            //        cilk_spawn spray(i, 0);
            //    }
			//	total_time = CLOCK() - start_time;
			//} else {
				//printf("Calling recursive_spawn()\n");
				fflush(stdout);
				start_time = CLOCK();
				cilk_spawn
				recursive_spawn(0, n_count);
				cilk_sync;
				total_time = CLOCK() - start_time;
				//printf("***Computation Complete***\n");
				fflush(stdout);
			//}

			long max_occurance = 0, next_addr, unique_keys = 0;
			if (nodelets_used < 8){
                for (i = 0; i < 8*100000; i++) {
                    next_addr = payload_state[i] & 4294967295;
                    if (next_addr > max_occurance) {
                        max_occurance = next_addr;
                    }

                    if (hash_table[i] != -1) {
                        unique_keys++;
                    }
                }
			} else {
                for (i = 0; i < 100000; i++) {
                    next_addr = payload_state[i] & 4294967295;
                    if (next_addr > max_occurance) {
                        max_occurance = next_addr;
                    }

                    if (hash_table[i] != -1) {
                        unique_keys++;
                    }
                }
            }
			execution_time = (double) total_time / CLOCK_RATE;
			printf("Datums Received: %ld\n", file_packets);
			printf("Unique Keys: %ld\n", unique_keys);
			printf("Max occurance of any key: %ld\n", max_occurance);
			printf("Event Count: %ld\n", stats[0]);
			printf("True Anomalies: %ld\n", stats[1]);
			printf("False Positives: %ld\n", stats[2]);
			printf("True Negatives: %ld\n", stats[3]);
			printf("False Negatives: %ld\n", stats[4]);
			printf("True Bias Flag Count: %ld\n", stats[5]);
			printf("Execution Time = %lf msec.\n", execution_time * 1000);
			printf("Datums per msec = %lf \n", (file_packets*nodelets_used) / (execution_time * 1000));
			fflush(stdout);

			cleanup();
		}
	}

    return 0;
}