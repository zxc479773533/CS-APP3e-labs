#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include "cachelab.h"

// Author: Yue Pan
// Replacement Policy: LRU

typedef struct {
    int v;
    int s;
    int E;
    int b;
} cache_size;

typedef struct {
    u_int valid;
    u_int tag;
    u_int time;
} cache_line;

typedef struct {
    char op;
    u_int addr;
    u_int index;
    u_int tag;
    u_int mem_size;
} operation;

typedef struct {
    int hits;
    int misses;
    int evictions;
} summary;

void print_help_info(void) {
    // Print the help inforation of this program
    printf("Usage: ./csim [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\n");
    printf("Examples:\n");
    printf("  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

int main(int argc, char **argv) {

    // get options
    char opt;
    int h = 0, v = 0, s = 0, S = 0, E = 0, b = 0, t = 0;
    char *filepath;
    while((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'h':
                h = 1;
                break;
            case 'v':
                v = 1;
                break;
            case 's':
                s = atoi(optarg);
                S = 1 << s;
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                t = 1;
                filepath = optarg;
                break;
            default:
                h = 1;
                break;
        }
    }
    if (h == 1) {
        print_help_info();
        exit(0);
    }
    if (s == 0 || E == 0 || b == 0 || t == 0) {
        printf("Arguments Error!\n\n");
        print_help_info();
        exit(1);
    }

    // Build cache
    summary my_summary;
    cache_size c_size;
    my_summary.hits = my_summary.misses = my_summary.evictions == 0;
    c_size.v = v, c_size.s = s, c_size.E = E, c_size.b = b;
    cache_line **cache = (cache_line **)malloc(sizeof(cache_line) * S * E);
    if (cache == NULL) {
        printf("Memory Error!\n");
        exit(1);
    }

    // Read file
    FILE *trace_file = fopen(filepath, "r");
    operation an_op;
    int time = 0;
    while (fscanf(trace_file, "%c %x,%u", &an_op.op, &an_op.addr, &an_op.mem_size) != EOF) {
        time++;
        an_op.index = (an_op.addr >> b) & ~(~0 << S);
        an_op.tag = an_op.addr >> (b + s);
        Caching();
    }
    free(cache);

    printSummary(my_summary.hits, my_summary.misses, my_summary.evictions);
    return 0;
}
