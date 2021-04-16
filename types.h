#ifndef LPIPE_TYPES
#define LPIPE_TYPES

#define TYPE_STRING 0
struct lpipe_string {
    char *data;
    size_t len;
};

#define TYPE_INT 1
struct lpipe_int {
    int val;
};

#define TYPE_FLOAT 2
struct lpipe_float {
    // float as in floating point, not _a_ float
    double val;
};

#define TYPE_NODE 3
struct lpipe_node {
    struct lpipe_lval *car;
    struct lpipe_lval *cdr;
};

#define TYPE_CALL 4
struct lpipe_call {
    // argc, argv
    void (*call)(void *, int, struct lpipe_lval *);
    void (*dealloc)(void *);
    void (*gc_flag)(void *);
    void *data;
};

#define TYPE_NIL 5
#define TYPE_OVER 6

struct lpipe_lval {
    int type;
    int gc_flag;
    union {
        struct lpipe_string str;
        struct lpipe_int i;
        struct lpipe_float f;
        struct lpipe_node node;
        struct lpipe_call call;
    } inner;
};

#endif
