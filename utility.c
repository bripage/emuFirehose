//
// Created by bpage1 on 1/24/20.
//
#include "utility.h"
#include "structdef.h"

void parse_args(int argc, char * argv[]) {
    size_t status;

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
}

long init_dist_end(long nodelet) {
	return packet_index[nodelet];
}

void get_data_and_distribute() {
    size_t status;
    long i, file_size, file_pin;
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

			nodelet = index_i % 8;

			index_n = packet_index[nodelet]; // get the element ID of the next empty nnz struct on the nodelet
			workload_dist[nodelet][index_n].address = packet_address;
			workload_dist[nodelet][index_n].val = packet_val;
			workload_dist[nodelet][index_n].flag = packet_flag;
			packet_index[nodelet]++; // increase nnz count for the nodelet we just added it to
			index_i++;
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

	// Mark list end by setting row and column values to -1
	/*
	for (i = 0; i < nodelet_count; i++) {
		index_n = packet_index[i];
		workload_dist[i][packet_index[i]].address = -1;
		workload_dist[i][packet_index[i]].val = -1;
        workload_dist[i][packet_index[i]].val = -1;
	}
	*/

	for (i = 0; i < nodelet_count; i++){
		printf("index[%d] = %d\n", i, packet_index[i]);
	}

	mw_replicated_init_multiple(&dist_end, init_dist_end);
}

void init(){
    long i, j;
    long nc = NODELETS();
    printf("Replicated init start.\n");
    fflush(stdout);
    mw_replicated_init(&nodelet_count, nc);

    unsigned long * s = (unsigned long *) mw_malloc1dlong(nodelet_count);
    printf("s allocated\n");
    fflush(stdout);
    if (h == NULL) {
        printf("Cannot allocate memory for stats array.\n");
        exit(1);
    }
    printf("checked s.\n");
    fflush(stdout);
    mw_replicated_init(&stats, s);
    for (i = 0; i < nodelet_count; i++){
        stats[i] = 0;
    }

    unsigned long * h = (unsigned long *) mw_malloc1dlong(100000);
	printf("h allocated\n");
	fflush(stdout);
	if (h == NULL) {
		printf("Cannot allocate memory for hash table.\n");
		exit(1);
	}
	printf("checked h.\n");
	fflush(stdout);
	mw_replicated_init(&hash_table, h);
	for (i = 0; i < 100000; i++){
		hash_table[i] = -1;
	}

	long * ah = (long *) mw_malloc1dlong(100000);
	printf("ah allocated\n");
	fflush(stdout);
	if (ah == NULL) {
		printf("Cannot allocate memory for address_hits.\n");
		exit(1);
	}
	printf("checked ah.\n");
	fflush(stdout);
	mw_replicated_init(&address_hits, ah);
	for (i = 0; i < 100000; i++){
		address_hits[i] = 0;
	}

    long * ps = (long *) mw_malloc1dlong(100000);
    printf("ps allocated\n");
    fflush(stdout);
    if (ps == NULL) {
        printf("Cannot allocate memory for payload_state.\n");
        exit(1);
    }
    printf("checked ps.\n");
    fflush(stdout);
    mw_replicated_init(&payload_state, ps);
    for (i = 0; i < 100000; i++){
        payload_state[i] = 0;
    }

    struct element ** wd;
    long packets_per_nodelet = ceil(PACKET_COUNT/nc);
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

}