#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include "cachelab.h"

// Author: Yue Pan
// Replacement Policy: LRU

enum {
    TYPE_MODIFY,
    TYPE_LOAD,
    TYPE_SAVE
};

enum {
    HIT,
    MISS,
    EVICTION
};

typedef struct {
    int visible;
    int set_num;
    int entry;
} cache_size;

typedef struct {
    unsigned long int valid;
    unsigned long int tag;
} cache_line;

typedef struct {
    char op;
    unsigned long int addr;
    unsigned long int index;
    unsigned long int tag;
    unsigned int mem_size;
} operation;

typedef struct {
    int hits;
    int misses;
    int evictions;
} summary;

void Caching(cache_line *cache, operation an_op, cache_size c_size, summary *my_summary) {

    int index;
    int type;
    int result;
    unsigned long int temp_rag;

    // get type
    switch (an_op.op) {
        case 'M':
            type = TYPE_MODIFY;
            break;
        case 'L':
            type = TYPE_LOAD;
            break;
        case 'S':
            type = TYPE_SAVE;
            break;
    }
    cache_line *pcache = cache + an_op.index * c_size.entry;

    // get result type
    result = EVICTION;
    for (index = 0; index < c_size.entry; index++) {
        if ((pcache + index)->valid == 0) {
            result = MISS;
            break;
        }
        if ((pcache + index)->tag == an_op.tag) {
            result = HIT;
            break;
        }
    }

    // LRU policy
    switch (result) {
        // if HIT, just refrash the time
        case HIT:
            temp_rag = (pcache + index)->tag;
            for ( ; index < c_size.entry - 1 && (pcache + index + 1)->valid == 1; index++) {
                (pcache + index)->tag = (pcache + index + 1)->tag;
            }
            (pcache + index)->tag = temp_rag;
            my_summary->hits++;
            break;
        // if MISS, add it to the cache
        case MISS:
            my_summary->misses++;
            (pcache + index)->valid = 1;
            (pcache + index)->tag = an_op.tag;
            break;
        // if EVICTION, delete the line who have minimum time and add it to the cache
        case EVICTION:
            my_summary->misses++;
            my_summary->evictions++;
            for (index = 0; index < c_size.entry - 1; index++) {
                (pcache + index)->tag = (pcache + index + 1)->tag;
            }
            (pcache + index)->tag = an_op.tag;
            break;
    }

    // the write in type modify is sure to hit
    if (type == TYPE_MODIFY)
        my_summary->hits++;
        
    // print type ( Only visible = 1 )
    if (c_size.visible == 1) {
        switch (type) {
            case TYPE_MODIFY:
                printf("M ");
                break;
            case TYPE_LOAD:
                printf("L ");
                break;
            case TYPE_SAVE:
                printf("S ");
                break;
        }
        printf("%lx,%d ", an_op.addr, an_op.mem_size);
        switch (result) {
            case HIT:
                printf("hit");
                break;
            case MISS:
                printf("miss");
                break;
            case EVICTION:
                printf("miss eviction");
                break;
        }
        if (type == TYPE_MODIFY)
            printf(" hit");
        printf("\n");
    }
}


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
    printf("  linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("  linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

int main(int argc, char **argv) {

    // get options
    char opt;
    int help = 0, visible = 0, set_power = 0, set_num = 0, entry = 0, block_power = 0, trace = 0;
    char *filepath;
    while((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'h':
                help = 1;
                break;
            case 'v':
                visible = 1;
                break;
            case 's':
                set_power = atoi(optarg);
                set_num = 1 << set_power;
                break;
            case 'E':
                entry = atoi(optarg);
                break;
            case 'b':
                block_power = atoi(optarg);
                break;
            case 't':
                trace = 1;
                filepath = optarg;
                break;
            default:
                help = 1;
                break;
        }
    }

    // Print help info
    if (help == 1) {
        print_help_info();
        exit(0);
    }
    if (set_power == 0 || entry == 0 || block_power == 0 || trace == 0) {
        printf("Arguments Error!\n\n");
        print_help_info();
        exit(1);
    }

    // Build cache
    summary my_summary;
    cache_size c_size;
    int index;
    my_summary.hits = my_summary.misses = my_summary.evictions = 0;
    c_size.visible = visible, c_size.set_num = set_num, c_size.entry = entry;
    cache_line *cache = (cache_line *)malloc(sizeof(cache_line) * set_num * entry);
    if (cache == NULL) {
        printf("Memory Error!\n");
        exit(1);
    }
    for (index = 0; index < set_num * entry; index++) {
        (cache + index)->valid = 0;
        (cache + index)->tag = 0xffffffff;
    }

    // Read file
    FILE *trace_file = fopen(filepath, "r");
    operation an_op;
    char line[80];
    char *pline = NULL;
    while (fgets(line, 80, trace_file) != NULL) {
        pline = line;
        if (*pline++ == 'I')
            continue;
        sscanf(pline, "%c %lx,%u", &an_op.op, &an_op.addr, &an_op.mem_size);
        an_op.index = (an_op.addr >> block_power) & ~(~0 << set_power);
        an_op.tag = an_op.addr >> (block_power + set_power);
        Caching(cache, an_op, c_size, &my_summary);
    }
    free(cache);

    // Print answer
    printSummary(my_summary.hits, my_summary.misses, my_summary.evictions);
    return 0;
}
