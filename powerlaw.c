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
unsigned long maxkeys = 100000;    // range of keys to generate from
unsigned int vseed = 98765;        // RN seed for generating values

// defaults for command-line switches

uint64_t npacket = 1000000;
int perpacket = 50;
int rate = 0;
uint64_t keyoffset = 1000000000;
int numgen = 1;
int whichgen = 0;
unsigned int kseed = 678912345;
uint32_t mask = 12345;

// timer, returns current time in seconds with usec precision

double myclock()
{
    double time;
    struct timeval tv;

    gettimeofday(&tv,NULL);
    time = 1.0 * tv.tv_sec + 1.0e-6 * tv.tv_usec;
    return time;
}

void generateDatums(long n){
    printf("spawned on %ld (n_used = %ld)\n", n, nodelets_used);
    fflush(stdout);

    numgen = nodelets_used;
    whichgen = n;
    struct packet * wdn = workload_dist[n];

    printf("local wdn set on %%ld\n", n);
    fflush(stdout);

    if (numPackets <= 0 || datumsPerPacket <= 0 || rate < 0 || whichgen < 0
        || whichgen >= numgen || kseed < 0 || mask <= 0){
        printf(stderr, "ERROR: invalid command-line switch\n");
        fflush(stdout);

        exit(1);
    }

    long i, j;

    // initialize RNG and seeds
    // use different keyoffsets in parallel, so key spaces are disjoint
    if (!kseed) kseed = (unsigned long) time(NULL);
    printf("kseed reset on %ld\n", n);
    fflush(stdout);
    kseed += whichgen;
    printf("kseed+whichgen on %ld\n", n);
    fflush(stdout);
    vseed += whichgen;
    printf("vseed set on %ld\n", n);
    fflush(stdout);
    srand(kseed);
    printf("srand set on %ld\n", n);
    fflush(stdout);
    keyoffset += whichgen*keyoffset;
    printf("keyoffset set on %ld\n", n);
    fflush(stdout);

    // create power-law distribution
    printf("initializing power-law distribution ...\n");
    fflush(stdout);

    power_law_distribution_t * power = power_law_initialize(concentration,maxkeys,RAND_MAX);

    // generate the datums in packets
    printf("starting generator ...\n");
    fflush(stdout);

    // plot
    //int *ycount = (int *) malloc(maxkeys*sizeof(int));
    //for (int i = 0; i < maxkeys; i++) ycount[i] = 0;
    double timestart = myclock();
    printf("timestart set on %ld\n", n);
    fflush(stdout);

    long w_index = 0;
    printf("w_index on %ld\n", n);
    fflush(stdout);

    for (i = 0; i < numPackets; i++) {
        //printf("packet loop: i = %ld (%ld)\n", i, n);
        fflush(stdout);
        // packet header
        //int offset = snprintf(buf,buflen,"packet %" PRIu64 "\n",i*numgen+whichgen);

        // generate one packet with perpacket datums
        uint64_t key;
        for (j = 0; j < datumsPerPacket; j++) {
            uint64_t rn = rand_r(&kseed);
            while ((key = power_law_simulate(rn,power)) >= maxkeys)
                rn = rand_r(&kseed);

            // plot
            //ycount[key]++;
            key += keyoffset;
            uint32_t value = 0;
            uint32_t bias = 0;
            if ((evahash((uint8_t*) &key,sizeof(uint64_t),mask) & 0xFF) == 0x11) {
                bias = 1;
                value = ((rand_r(&vseed) & 0xF) == 0);
            } else value = rand_r(&vseed) & 0x1;

            //offset += snprintf(buf+offset,buflen-offset, "%" PRIu64 ",%u,%u\n",key,value,bias);
            //printf("wdn[%ld] = %zu, %ld, %ld, (%ld)\n", w_index, key, value, bias, n);
            //fflush(stdout);

            if (w_index > datumsPerPacket*numPackets){
                printf("ERROR: %ld > %ld", w_index, datumsPerPacket*numPackets);
                fflush(stdout);
            }

            wdn[w_index].address = key;
            wdn[w_index].val = value;
            wdn[w_index].flag = bias;
            w_index++;
        }
        //printf("out of datum loop %ld (%ld)\n", i, n);
        //fflush(stdout);

        // sleep if rate is throttled
        /*
        if (rate) {
            double n = 1.0*(i+1)*datumsPerPacket;
            double elapsed = myclock() - timestart;
            double actual_rate = n/elapsed;
            if (actual_rate > rate) {
                double delay = n/rate - elapsed;
                usleep(1.0e6*delay);
            }
        }
        */
    }
    printf("after packet loop on %ld\n", n);
    fflush(stdout);
/*
    double timestop = myclock();
    printf("after get stop time (%ld)\n", n);
    fflush(stdout);
    unsigned long ndatum = numPackets * datumsPerPacket;
    printf("after ndatum calc (%ld)\n", n);
    fflush(stdout);
/*
    printf(stdout,"packets emitted = %zu\n",npacket);
    fflush(stdout);
    printf(stdout,"datums emitted = %zu\n",ndatum);
    fflush(stdout);
    printf(stdout,"elapsed time (secs) = %g\n",timestop-timestart);
    fflush(stdout);
    printf(stdout,"generation rate (datums/sec) = %g\n", ndatum/(timestop-timestart));
    fflush(stdout);
*/
    power_law_destroy(power);
}
