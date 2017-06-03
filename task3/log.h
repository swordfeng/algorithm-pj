enum OP {
    INS = 0,
    SUB = 1,
    DEL = 2
};

/* sizeof(Log) == 8 */
#pragma pack(push, 1)
struct Log {
    OP op : 2;
    unsigned int x : 17;
    char c : 8;
    unsigned long long last : 37;
};
#pragma pack(pop)
