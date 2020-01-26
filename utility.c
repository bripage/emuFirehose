//
// Created by bpage1 on 1/24/20.
//
#include "utility.h"

void parse_args(struct spmv_args * spmv_args, int argc, char * argv[]) {
    size_t status;

    // Open Input File
    ifp = fopen(argv[1], "r");
    if (ifp == NULL) {
        printf("Failed to open input file.\n");
        exit(1);
    }

    binary_input = atoi(argv[2]);
    printf("binary_input = %ld\n", binary_input);
    fflush(stdout);

    args->PACKET_COUNT = atol(argv[3]);
    printf("Packet Count = %ld\n", spmv_args->PACKET_COUNT);
    fflush(stdout);
}

void get_data_and_distribute() {
    size_t status;
    long file_size, file_pin;
    unsigned long packet_id;
    char val, flag;
    char *buffer;

    // obtain file size:
    file_pin = ftell(ifp);
    fseek(ifp, 0, SEEK_END);
    file_size = ftell(ifp);
    file_size -= file_pin;
    rewind(ifp);

    // allocate memory for buffer
    printf("Starting Buffer Allocation\n");
    fflush(stdout);
    buffer = (char *) malloc(sizeof(char) * (file_size));
    printf("Done allocating buffer\n");
    fflush(stdout);
    if (buffer == NULL) {
        printf("Failed to allocate buffer.\n");
        exit(1);
    }

    status = fread(buffer, 1, file_size, ifp);
    // copy the file into the buffer
    if (status != file_size) {
        printf("Error in reading file.\n");
        exit(1);
    }
    printf("file copied into buffer\n");
    fflush(stdout);


}

void init(struct * args){
    long i, j;
    long nc = NODELETS();
    printf("Replicated init start.\n");
    fflush(stdout);
    mw_replicated_init(&nodelet_count, nc);
    mw_replicated_init(&PACKET_COUNT, args->PACKET_COUNT);

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

    packet_index = (long *) malloc(nodelet_count*sizeof(long));
    for (i=0; i < nodelet_count; i++){
        packet_index[i]=0;
    }

    get_data_and_distribute();
}

void cleanup(){

}