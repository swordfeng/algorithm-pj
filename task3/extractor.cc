#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <cstring>

#include <lzo/minilzo.h>

#include "log.h"

using namespace std;

const size_t LOG_POOL_SIZE = 300000 * 16 * 8;

Log pool_data[LOG_POOL_SIZE];
uint8_t pool_data_lzo[LOG_POOL_SIZE * sizeof(Log) + 2048];
uint8_t lzo_work[LZO1X_MEM_DECOMPRESS];

long long cur_pool = -1;

void open_pool(long long pool) {
    if (cur_pool == pool) return;
    char filename[100];
    sprintf(filename, "%lld.data.lzo", pool * LOG_POOL_SIZE);
    FILE *f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "file not found: %s\n", filename);
        exit(255);
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    fread(pool_data_lzo, sz, 1, f);
    fclose(f);
    lzo_uint out_size = sizeof(pool_data);
    lzo1x_decompress(pool_data_lzo, sz, (uint8_t *)pool_data, &out_size, lzo_work);
    if (out_size != sizeof(pool_data)) {
        exit(254);
    }
    cur_pool = pool;
}

Log extract(long long ref) {
    --ref;
    long long pool = ref / LOG_POOL_SIZE;
    long long pos = ref % LOG_POOL_SIZE;
    open_pool(pool);
    return pool_data[pos];
}

int count = 0;
void printlog(long long ref) {
    fprintf(stderr, "%d\t%lld\n", ++count, ref);
    if (ref == 0) return;
    Log l = extract(ref);
    printlog(l.last);
    switch (l.op) {
        case INS: printf("INS %d %c\n", l.x, l.c); break;
        case SUB: printf("SUB %d %c\n", l.x, l.c); break;
        case DEL: printf("DEL %d\n", l.x); break;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "%s <log_reference>\n", argv[0]);
    }
    long long logref;
    sscanf(argv[1], "%llu", &logref);
    printlog(logref);
    return 0;
}
