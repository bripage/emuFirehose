//
// Created by bpage1 on 2/14/20.
//
// Stream generator #1 = biased power-law generator

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

//#include "../include/udp_throw.h"
#include "powerlaw.h"
#include "evahash.h"
#include "structdef.h"

// UDP ports

//udp_throw_t *udpt = NULL;

// hard-coded settings

#define DEFAULT_PORT 55555    // default port to write to
#define STOPRATE 100          // per sec rate of STOP packets

double concentration = 0.5;        // slope of power-law distribution
unsigned long maxkeys = 6400000;    // range of keys to generate from
//unsigned int vseed = 1337;        // RN seed for generating values

// defaults for command-line switches

uint64_t npacket = 1000000;
int perpacket = 50;
int rate = 0;
uint64_t keyoffset = 1000000000;
int numgen = 1;
int whichgen = 0;
//unsigned int kseed = 696969;
uint32_t mask = 12345;

// timer, returns current time in seconds with usec precision
double myclock(){
    double time;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    time = 1.0 * tv.tv_sec + 1.0e-6 * tv.tv_usec;
    return time;
}

void make_packets(long nodelet, long kseed, long vseed, long keyoffset, power_law_distribution_t * power) {
    struct packet *wdn = workload_dist[nodelet];
    long i, j, w_index = 0, dat_id;
    uint64_t key;
    uint32_t value = 0;
    uint32_t bias = 0;
    uint64_t rn;
    //long maxDats = numDatums;

    //for (i = 0; i < nodelets_used; i++) {
    for (j = 0; j < numDatums; j++) {
        //dat_id = (j * nodelets_used)+nodelet;
        // generate one packet with perpacket datums
        rn = rand_r(&kseed);
        while ((key = power_law_simulate(rn, power)) >= maxkeys)
            rn = rand_r(&kseed);

        key += keyoffset;
        value = 0;
        bias = 0;
        if ((evahash((uint8_t *) &key, sizeof(uint64_t), mask) & 0xFF) == 0x11) {
            bias = 1;
            value = ((rand_r(&vseed) & 0xF) == 0);
        } else value = rand_r(&vseed) & 0x1;

        if (w_index > numDatums) {
            printf("ERROR: %ld > %ld", w_index, numDatums);
            fflush(stdout);
        }

        REMOTE_ADD(&wdn[w_index].address, key);
        REMOTE_ADD(&wdn[w_index].val, value);
        REMOTE_ADD(&wdn[w_index].flag, bias);
        //REMOTE_ADD(&workload_dist[i][j].address, key);
        //REMOTE_ADD(&workload_dist[i][j].val, value);
        //REMOTE_ADD(&workload_dist[i][j].flag, bias);

        w_index++;
    }
    //}

    printf("after packet loop on %ld\n", nodelet);
    fflush(stdout);
}

void generateDatums(long n){
    // create power-law distribution
    printf("initializing power-law distribution ...\n");
    fflush(stdout);

    power_law_distribution_t * power = power_law_initialize(concentration, maxkeys, RAND_MAX);

    //uint64_t myoffset;
    cilk_for (long nodelet = 0; nodelet < nodelets_used; nodelet++) {
        unsigned int kseed = 696969 * (nodelet+1);
        unsigned int vseed = 1337*(nodelet+1);

        if (!kseed) kseed = (unsigned long) time(NULL);
        printf("kseed reset on %ld\n", nodelet);
        fflush(stdout);
        //kseed *= nodelet;
        printf("kseed = %zu on %ld\n", kseed, nodelet);
        fflush(stdout);
        //vseed *= nodelet;
        printf("vseed = %zu on %ld\n", vseed, nodelet);
        fflush(stdout);
        srand(kseed);
        printf("srand set on %ld\n", nodelet);
        fflush(stdout);
        //myoffset = nodelet*keyoffset;
        //printf("keyoffset %zu set on %ld\n", myoffset, nodelet);
        //fflush(stdout);

        // generate the datums in packets
        printf("starting generator ...\n");
        fflush(stdout);
        make_packets(nodelet, kseed, vseed, keyoffset, power);
    }

    power_law_destroy(power);
}