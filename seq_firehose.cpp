//
// Created by Brian Page on 2020-01-31.
//

#include <math.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <algorithm>
#include <functional>

struct packet {
	uint64_t address;
	int64_t val;
	int64_t flag;
};

int main (int argc, char **argv){

	size_t status;
	int64_t i, file_size, file_pin;
	uint64_t packet_address;
	int64_t packet_val, packet_flag, file_packets;
	int64_t nodelet, index_i = 0;
	FILE *ifp;

	// Open Input File
	ifp = fopen(argv[1], "rb");
	if (ifp == NULL) {
		printf("Failed to open input file.\n");
		exit(1);
	}

	// get packet count from file header
	int64_t fbuf;
	status = fread(&fbuf, sizeof(int64_t), 1, ifp);
	if (status != 1) {
		printf("Error could not read data header\n");
		fflush(stdout);
		exit(1);
	}
	printf("Packet Count = %lld\n", fbuf);
	fflush(stdout);
	file_packets = fbuf;

	int64_t chunk_elements = 40000000;
	int64_t chunk_size = chunk_elements * sizeof(struct packet);
	int64_t chunk_count = 0, final_chunk_size = 0, final_chunk_elements = 0;
	struct packet* binBuffer;
	std::vector<packet> datums;

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
	printf("chunk_elements = %lld, chunk_size = %lld, chunk_count = %lld, final_chunk_elements = %lld, final_chunk_size = %lld\n", chunk_elements, chunk_size, chunk_count, final_chunk_elements, final_chunk_size);
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
		printf("Error in reading file. %zu != %lld\n", status, chunk_size);
		fflush(stdout);
		exit(1);
	}
	printf("file copied into buffer\n");
	fflush(stdout);

	int64_t bufPtr;
	for (i = 0; i < chunk_count; i++) {
		printf("%lld\n", i);
		fflush(stdout);
		for (bufPtr = 0; bufPtr < chunk_elements; bufPtr++) {
			packet_address = binBuffer[bufPtr].address;
			packet_val = binBuffer[bufPtr].val;
			packet_flag = binBuffer[bufPtr].flag;

			packet p;
			p.address= packet_address;
			p.val = packet_val;
			p.flag = packet_flag;

			datums.push_back(p);
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
					printf("Error in reading file chunk %lld\n", i);
					exit(1);
				}
				printf("file chunk %lld copied into buffer\n", i);
				fflush(stdout);
			}
		}
	}
	free(binBuffer);
	printf("done reading %lld datums from file\n", (int64_t) datums.size());
	fflush(stdout);

	int64_t* hash_table;
	int64_t* payload_state;
	int64_t* address_hits;
	hash_table = (int64_t *) malloc(100000*sizeof(int64_t));
	payload_state = (int64_t *) malloc(100000*sizeof(int64_t));
	address_hits = (int64_t *) malloc(100000*sizeof(int64_t));
	for (i = 0; i < 100000; i++){
		hash_table[i] = -1;
		payload_state[i] = 0;
		address_hits[i] = 0;
	}

	uint64_t addr;
	int64_t val, flag;
	int64_t hash, j, hits = 0, payload = 0, swap_state, state = 0, temp = 0;

	int64_t event_count = 0,
			true_anomalies = 0,
			false_positives = 0,
			true_negatives = 0,
			false_negatives = 0,
			bias_count = 0;

	for (i = 0; i < datums.size(); i++){
		addr = datums[i].address;
		val = datums[i].val;
		flag = datums[i].flag;

		if (flag == 1){
			bias_count++;
		}

		hash = addr % 100000; //inline this
		j = hash;

		if (hash_table[j] == -1 || hash_table[j] == addr){  // found an empty slot on the first try (woohoo)
			hash_table[j] = addr;
			// insert and update state table
			address_hits[j]++;

			if (address_hits[j] % 24 == 0) {
				event_count++;
				payload = payload_state[j];
				payload_state[j] = 0;

				if (payload <= 4 && flag == 1){
					true_anomalies++;
				} else if (payload <= 4 && flag == 0){
					false_positives++;
				} else if (payload > 4 && flag == 0){
					true_negatives++;
				} else if (payload > 4 && flag == 1){
					false_negatives++;
				}
			} else {
				payload_state[j]++;
			}

		} else {    // slot taken, find an empty one
			while (hash_table[j] != -1 || hash_table[j] != addr){
				if (j+1 == 100000) {
					j = 0;
				} else {
					j++;
				}

				if (j == hash){
					printf("ERROR: Hash table FULL\n");
					fflush(stdout);
					exit(1);
				}
			}

			// now that we have either found the key in the hashtable or located an
			// empty slot, add or update the state for the given location and key
			address_hits[j]++;
			state = payload_state[j] + 4294967297; // increment both high 32 and low bits by one.
			hits = (payload_state[j] & 4294967295) +1;
			printf("hits = %lld\n", hits);
			fflush(stdout);
			if (hits == 24) {
				event_count++;

				payload_state[j] = 0;
				payload = state >> 32;

				if (payload <= 4 && flag == 1){
					true_anomalies++;
				} else if (payload <= 4 && flag == 0){
					false_positives++;
				} else if (payload > 4 && flag == 0){
					true_negatives++;
				} else if (payload > 4 && flag == 1){
					false_negatives++;
				}
			}
		}
	}

	int64_t max_occurance = 0, next_addr, unique_keys = 0;
	for (i = 0; i < 100000; i++){
		next_addr = address_hits[i];
		if (next_addr > max_occurance){
			max_occurance = next_addr;
		}

		if (hash_table[i] != -1){
			unique_keys++;
		}
	}
	printf("Datums Received: %lld\n", file_packets);
	printf("Unique Keys: %lld\n", unique_keys);
	printf("Max occurance of any key: %lld\n", max_occurance);
	printf("Event Count: %lld\n",event_count);
	printf("True Anomalies: %lld\n", true_anomalies);
	printf("False Positives: %lld\n", false_positives);
	printf("True Negatives: %lld\n", true_negatives);
	printf("False Negatives: %lld\n", false_negatives);
	printf("True Bias Flag Count: %lld\n", bias_count);
	fflush(stdout);

	return 0;
}