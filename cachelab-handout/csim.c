#include "cachelab.h"
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

int main(int argc, char** argv){
    int opt, s, E, b, S;
    int verbose = 0;
    char * t;

    FILE * pFile; // pointer to FILE object
    int * cacheBuff; // pointer to cache buffer

    char identifier;
    unsigned long address;
    int size;
    unsigned long CT;
    unsigned CI;

    int hit_cnt = 0, miss_cnt = 0, eviction_cnt = 0;

    int * head;
    int cacheline_bytes;
    int cacheset_bytes;
    int tag_bytes;
    int hit, eviction;
    int time_stamp = 0;
    int LRU = 0;
    int least_timestamp;
    int hit_line;

    int i, j;
    /* looping over arguments */
    while(-1 != (opt = getopt(argc, argv, "s:E:b:t:v"))){
        /* determine which argument it’s processing */
        switch(opt) {
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                t = optarg;
                break;
            case 'v':
                verbose = 1;
                break;
            default:
                printf("wrong argument\n");
                break;
        }
    }
    S = (2 << s);
    tag_bytes = 64 - b - s;
    cacheline_bytes = 1 + tag_bytes + 1; // bytes in each cache line: valid + tag  + LRU stamp
    cacheset_bytes = E * cacheline_bytes; // bytes in each cache set
    // Allocate space for cache buffer: set numbers * cache line numbers per set * bytes per cache line
    cacheBuff = malloc(sizeof(int) * S * E * cacheline_bytes); 
    pFile = fopen(t, "r");
    if (pFile == NULL) {
        printf("No such file.\n");
        return 0;
    }
    // Reading lines like "M 20,1" or "L 19, 3"
    while (fscanf(pFile, "%*c %c %lx,%d", &identifier, &address, &size) > 0) {
        if (identifier == 'I') {
            continue;
        }
        time_stamp += 1;
        CI = (address >> b) & ((2 << (s - 1)) - 1);
        CT = (address >> (b + s)) & ((2 << (64 - b - s - 1)) - 1);
        head = cacheBuff + CI * cacheset_bytes; // point to head of the specific set
        for (i = 0; i < E; i++) {
            if (*(head + i * cacheline_bytes) == 1) {
                for (j = 0; j < tag_bytes; j++) {
                    if (*(head + i * cacheline_bytes + (tag_bytes - j)) != ((CT >> j) & 0x1)) {
                        // tag bit not match, search next cache line
                        break;
                    }
                }
                if (j == tag_bytes) {
                    // valid bit is set && cache line tag bits match address tag bits
                    hit = 1;
                    break;
                }
            }
        }
        hit_line = i;
        // if miss, search for the least used block
        least_timestamp = *(head + 1 + tag_bytes);
        for (i = 0; i < E; i++) {
            if (*(head + i * cacheline_bytes + 1 + tag_bytes) <= least_timestamp) {
                least_timestamp = *(head + i * cacheline_bytes + 1 + tag_bytes);
                LRU = i;
            }
        }
        if (hit == 1) {
            hit_cnt += 1;
            // record least recent use
            *(head + hit_line * cacheline_bytes + 1 + tag_bytes) = time_stamp;
        } else {
            // fetch data from next level memory and set cache
            // use LRU policy to deal with conflict
            if (*(head + LRU * cacheline_bytes) == 1) {
                eviction = 1;
                eviction_cnt += 1;
            }
            *(head + LRU * cacheline_bytes) = 1; // set valid bit 
            // set tag bits
            for (i = 0; i < tag_bytes; i++) {
                *(head + LRU * cacheline_bytes + (tag_bytes - i)) = ((CT >> i) & 0x1);
            }
            *(head + LRU * cacheline_bytes + 1 + tag_bytes) = time_stamp; // update time stamp
            miss_cnt += 1;
        }
        if (identifier == 'L' || identifier == 'S') {
            if (verbose == 1) {
                if (hit == 1) {
                    printf("%c %lx,%d hit\n", identifier, address, size);
                    printf("CT=%lx, CI=%d\n\n", CT, CI);
                } else {
                    if (eviction == 1) {
                        printf("%c %lx,%d miss eviction\n", identifier, address, size);
                        printf("CT=%lx, CI=%d\n\n", CT, CI);
                    } else {
                        printf("%c %lx,%d miss\n", identifier, address, size);
                        printf("CT=%lx, CI=%d\n\n", CT, CI);
                    }
                }
            }
        } else if (identifier == 'M') {
            hit_cnt += 1;
            if (verbose == 1) {
                if (hit == 1) {
                    printf("%c %lx,%d hit hit\n", identifier, address, size);
                    printf("CT=%lx, CI=%d\n\n", CT, CI);
                } else {
                    if (eviction == 1) {
                        printf("%c %lx,%d miss eviction hit\n", identifier, address, size);
                        printf("CT=%lx, CI=%d\n\n", CT, CI);
                    } else {
                        printf("%c %lx,%d miss hit\n", identifier, address, size);
                        printf("CT=%lx, CI=%d\n\n", CT, CI);
                    }
                }
            }
        }
        hit = 0;
        eviction = 0;
    }
    fclose(pFile);
    free(cacheBuff);
    printSummary(hit_cnt, miss_cnt, eviction_cnt);
    return 0;
}
