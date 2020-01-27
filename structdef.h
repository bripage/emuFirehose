//
// Created by bpage1 on 1/27/20.
//

#ifndef EMUFIREHOSE_STRUCTS_H
#define EMUFIREHOSE_STRUCTS_H

// Structure to hold packets in workload list
struct packet {
    unsigned long address;
    long val;
    long flag;
};

#endif //EMUFIREHOSE_STRUCTS_H