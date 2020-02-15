//
// Created by bpage1 on 1/24/20.
//
#include "utility.h"
#include "structdef.h"

void parse_args(int argc, char * argv[]) {
    size_t status;
/*
    // Open Input File
    ifp = fopen(argv[1], "rb");
    if (ifp == NULL) {
        printf("Failed to open input file.\n");
        exit(1);
    }

    // get packet count from file header
	long fbuf;
	status = fread(&fbuf, sizeof(long), 1, ifp);
	if (status != 1) {
		printf("Error could not read data header\n");
		fflush(stdout);
		exit(1);
	}
	printf("Packet Count = %ld\n", fbuf);
	fflush(stdout);
	file_packets = fbuf;
*/
	long nu = atoi(argv[1]);
	mw_replicated_init(&nodelets_used, nu);
	printf("Nodelets used = %ld\n", nodelets_used);
	fflush(stdout);

    long np = atoi(argv[2]);
    mw_replicated_init(&numPackets, np);
    printf("Packet Count = %ld\n", numPackets);
    fflush(stdout);

    long dpp = atoi(argv[3]);
    mw_replicated_init(&datumsPerPacket, dpp);
    printf("Datums Per Packet = %ld\n", datumsPerPacket);
    fflush(stdout);
    file_packets = datumsPerPacket * numPackets;
}

long init_dist_end(long nodelet) {
	return packet_index[nodelet];
}

void recursive_init_spawn(long low, long high){
    long i;
    long nodelet = NODE_ID();
    printf("spawned on %ld.\n", nodelet);
    fflush(stdout);
    // Want a grainsize of 1 to spawn a threadlet at each nodelet
    for (;;) {
        unsigned long count = high - low;
        // spawn a threadlet at each nodelet
        if (count <= 1) break;
        // Invariant: count >= 2
        unsigned long mid = low + count / 2;
        cilk_migrate_hint(&hash_table[mid]);
        cilk_spawn recursive_init_spawn(mid, high);

        high = mid;
    }

    cilk_spawn generateDatums(nodelet);
}

void get_data_and_distribute() {
    size_t status;
    long i;

/*
    long i, file_size, file_pin, j;
    unsigned long packet_address;
    long packet_val, packet_flag;
    long nodelet, index_i = 0;

	long chunk_elements = 40000000 ;
	long chunk_size = chunk_elements * sizeof(struct packet);
	long chunk_count = 0, final_chunk_size = 0, final_chunk_elements = 0;
	struct packet* binBuffer;

	if (file_packets < 40000000){
		chunk_size = file_packets  * sizeof(struct packet);
		chunk_elements = file_packets;
		chunk_count = 1;
		final_chunk_size = 0;
	} else {
		file_size = (file_packets+1) * sizeof(struct packet);
		chunk_count = (file_packets) / chunk_elements;
		final_chunk_elements = file_packets - (chunk_count * chunk_elements);
		final_chunk_size = final_chunk_elements * sizeof(struct packet);

		if (final_chunk_size != 0){
			chunk_count++;
		}
	}
	printf("chunk_elements = %ld, chunk_size = %ld, chunk_count = %ld, final_chunk_elements = %ld, final_chunk_size = %ld\n", chunk_elements, chunk_size, chunk_count, final_chunk_elements, final_chunk_size);
	fflush(stdout);
	binBuffer = (struct packet *) malloc(chunk_size);
	printf("Done allocating initial buffer chunk\n");
	fflush(stdout);
	if (binBuffer == NULL) {
		printf("Failed to allocate initial buffer chunk.\n");
		exit(1);
	}

	status = fread(binBuffer, sizeof(struct packet), chunk_elements, ifp);
	if (status != chunk_elements) {
		printf("Error in reading file. %ld != %ld\n", status, chunk_size);
		fflush(stdout);
		exit(1);
	}
	printf("file copied into buffer\n");
	fflush(stdout);

	long bufPtr, index_n;
	for (i = 0; i < chunk_count; i++) {
		printf("%ld\n", i);
		fflush(stdout);
		for (bufPtr = 0; bufPtr < chunk_elements; bufPtr++) {
			packet_address = binBuffer[bufPtr].address;
			packet_val = binBuffer[bufPtr].val;
			packet_flag = binBuffer[bufPtr].flag;

			//j = index_i % nodelets_used;

			for (j = 0; j < nodelets_used; j++) {
				index_n = packet_index[j]; // get the element ID of the next empty nnz struct on the nodelet
				workload_dist[j][index_n].address = packet_address;
				workload_dist[j][index_n].val = packet_val;
				workload_dist[j][index_n].flag = packet_flag;
				packet_index[j]++; // increase nnz count for the nodelet we just added it to
				index_i++;
			}
		}

		if (chunk_count > 1 && i != chunk_count-1) {
			if (i+1 == chunk_count-1) {
				printf("about to free buffer\n");
				fflush(stdout);
				free(binBuffer);
				printf("allocating buffer for final chunk\n");
				fflush(stdout);
				binBuffer = (struct packet *) malloc(final_chunk_size);

				status = fread(binBuffer, 1, final_chunk_size, ifp);
				if (status != final_chunk_size) {
					printf("Error in reading final file chunk\n");
					exit(1);
				}
				printf("final file chunk copied into buffer\n");
				fflush(stdout);
				chunk_elements = final_chunk_elements;
			} else {
				printf("reading in next chunk\n");
				fflush(stdout);
				status = fread(binBuffer, 1, chunk_size, ifp);
				if (status != chunk_size) {
					printf("Error in reading file chunk %ld\n", i);
					exit(1);
				}
				printf("file chunk %ld copied into buffer\n", i);
				fflush(stdout);
			}
		}
	}
	free(binBuffer);
	printf("done reading matrix from buffer\n");
	fflush(stdout);
*/
    printf("Migrating back to nodelet 0.\n");
    fflush(stdout);
    MIGRATE(&payload_state[0]);

    cilk_spawn recursive_init_spawn(0, nodelets_used);
    cilk_sync;

	for (i = 0; i < nodelet_count; i++){
		printf("index[%d] = %d\n", i, packet_index[i]);
	}

	mw_replicated_init_multiple(&dist_end, init_dist_end);
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

    unsigned long * h;
    //if (nodelets_used < 8){
    //    h = (unsigned long *) mw_malloc1dlong(8*100000);
    //} else {
        h = (unsigned long *) mw_malloc1dlong(100000);
    //}
    printf("h allocated\n");
    fflush(stdout);
    if (h == NULL) {
        printf("Cannot allocate memory for hash table.\n");
        exit(1);
    }
    printf("checked h.\n");
    fflush(stdout);
    mw_replicated_init(&hash_table, h);
    //if (nodelets_used < 8){
    //    for (i = 0; i < 8*100000; i++) {
    //        hash_table[i] = -1;
    //    }
    //} else {
    for (i = 0; i < 100000; i++) {
        hash_table[i] = -1;
    }
    //}

    long * ps;
    //if (nodelets_used < 8){
    //    ps = (unsigned long *) mw_malloc1dlong(8*100000);
    //} else {
        ps = (unsigned long *) mw_malloc1dlong(100000);
    //}
    printf("ps allocated\n");
    fflush(stdout);
    if (ps == NULL) {
        printf("Cannot allocate memory for hash table.\n");
        exit(1);
    }
    printf("checked ps.\n");
    fflush(stdout);
    mw_replicated_init(&payload_state, ps);
    //if (nodelets_used < 8){
    //    for (i = 0; i < 8*100000; i++) {
    //        payload_state[i] = 0;
    //    }
    //} else {
        for (i = 0; i < 100000; i++) {
            payload_state[i] = 0;
        }
    //}

    struct element ** wd;
    long packets_per_nodelet;
    //packets_per_nodelet = ceil(file_packets/nodelets_used);
    packets_per_nodelet = file_packets;
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

    packet_index = (long *) malloc(nodelet_count * sizeof(long));
    for (i = 0; i < nodelet_count; i++){
        packet_index[i] = 0;
    }

    get_data_and_distribute();
}

void cleanup(){
	long i;
    //if (nodelets_used < 8) {
    //    for (i = 0; i < 8*100000; i++) {
    //        hash_table[i] = -1;
    //        payload_state[i] = 0;
    //    }
    //} else {
        for (i = 0; i < 100000; i++) {
            hash_table[i] = -1;
            payload_state[i] = 0;
        }
    //}

    for (i = 0; i < nodelet_count; i++){
		stats[i] = 0;
	}
}