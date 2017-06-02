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

using namespace std;

enum OP {
    INS = 0,
    SUB = 1,
    DEL = 2
};

struct Log {
    OP op;
    int x;
    char c;
    long long last;
};

const size_t LOG_POOL_SIZE = 300000 * 16 * 8;

uint8_t pool_data[LOG_POOL_SIZE * 7];
uint8_t pool_data_lzo[LOG_POOL_SIZE * 7 + 1024];
uint8_t lzo_work[LZO1X_MEM_DECOMPRESS];

long long cur_pool = -1;

void open_pool(long long pool) {
    if (cur_pool == pool) return;
    char filename[100];
    sprintf(filename, "%lld.data.lzo", pool * LOG_POOL_SIZE);
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("file not found: %s\n", filename);
        exit(255);
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    fread(pool_data_lzo, sz, 1, f);
    lzo_uint out_size = sizeof(pool_data);
    lzo1x_decompress(pool_data_lzo, sz, pool_data, &out_size, lzo_work);
    if (out_size != sizeof(pool_data)) {
        exit(254);
    }
    cur_pool = pool;
}

char unconvchar(char c) {
    switch (c) {
        case 0: return 'A';
        case 1: return 'C';
        case 2: return 'G';
        case 3: return 'T';
    }
    exit(255);
}

Log extract(long long ref) {
    --ref;
    Log l;
    long long pool = ref / LOG_POOL_SIZE;
    long long pos = ref % LOG_POOL_SIZE;
    open_pool(pool);
    uint64_t line = 0;
    memcpy(&line, &pool_data[pos * 7], 7);
    l.last = line & ((1LL<<35)-1);
    l.c = unconvchar(char((line>>35) & ((1LL<<2)-1)));
    l.x = int((line>>37) & ((1LL<<14)-1));
    l.op = (OP)(line>>51);
    return l;
}

void printlog(long long ref) {
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
        printf("%s <log_reference>\n", argv[0]);
    }
    long long logref;
    sscanf(argv[1], "%lld", &logref);
    printlog(logref);
    return 0;
}
