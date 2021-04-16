#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

struct lpipe_gc_ent {
    struct lpipe_gc_ent *next;
    struct lpipe_lval val;
}

struct lpipe_genv_ent {
    struct lpipe_genv *next;
    struct lpipe_lval key;
    struct lpipe_lval value;
}

struct lpipe_context {
    struct lpipe_gc_ent *gc_head;
    struct lpipe_gc_ent *gc_tail;
    struct lpipe_genv_ent *genv_head;
    struct lpipe_genv_ent *genv_tail;
}

struct lpipe_context *lpipe_new_context() {
    struct lpipe_context *ret = malloc(sizeof struct lpipe_context);
    memset(ret, 0, sizeof struct lpipe_context);
}

// does not take care of linked list pointer modification
void lpipe_free_gc_ent(struct lpipe_gc_ent *ent) {
    switch (ent->val.type) {
        case TYPE_STRING:
            free(ent->val.inner.str.data);
            break;
        case TYPE_CALL:
            ent->val.inner.call.dealloc(ent->val.inner.call.data);
    }
    free(ent);
}

void lpipe_free_context(struct lpipe_context *ctx) {
    // free gc memory
    struct lpipe_gc_ent *gc_cur = ctx->gc_head;
    while (gc_cur) {
        struct lpipe_gc_ent *gc_next = gc_cur->next;
        lpipe_free_gc_ent(gc_cur);
        gc_cur = gc_next;
    }
}

void lpipe_run_gc(struct lpipe_context *ctx) {
    

struct lpipe_lval *lpipe_alloc_raw(struct lpipe_context *ctx, struct lpipe_lval v) {
    struct lpipe_gc_ent *ent = malloc(sizeof struct lpipe_gc_ent);
    ent->next = NULL;
    ent->val = v;
    ctx->gc_tail.next = ent;
    ctx->gc_tail = ent;
    return &ent->val;
}

struct lpipe_lval *lpipe_alloc_str(struct lpipe_context *ctx, char *data, size_t len) {
    struct lpipe_lval v = {
        .type = TYPE_STRING,
        .gc_flag = 0,
        .inner.str = {.data = data, .len = len}
    };
    return lpipe_alloc_raw(ctx, v);
}

struct lpipe_lval *lpipe_alloc_zstr(struct lpipe_context *ctx, char *s) {
    return lpipe_alloc_str(ctx, s, strlen(s));
}

struct lpipe_lval *lpipe_alloc_int(struct lpipe_context *ctx, int i) {
    struct lpipe_lval v = {
        .type = TYPE_INT,
        .gc_flag = 0,
        .inner.i.val = i
    };
    return lpipe_alloc_raw(ctx, v);
}

struct lpipe_lval *lpipe_alloc_floatd(struct lpipe_context *ctx, double d) {
    struct lpipe_lval v = {
        .type = TYPE_FLOAT,
        .gc_flag = 0,
        .inner.f.val = d
    };
    return lpipe_alloc_raw(ctx, v);
}

struct lpipe_lval *lpipe_alloc_floatf(struct lpipe_context *ctx, float f) {
    return lpipe_alloc_floatd(ctx, f);
}

struct lpipe_lval *lpipe_alloc_node(struct lpipe_context *ctx, struct lpipe_lval *car, struct lpipe_lval *cdr) {
    struct lpipe_lval v = {
        .type = TYPE_NODE,
        .gc_flag = 0,
        .inner.node = {.car = car, .cdr = cdr}
    };
    return lpipe_alloc_raw(ctx, v);
}

struct lpipe_lval *lpipe_alloc_c_call(struct lpipe_context *ctx, struct lpipe_call call) {
    struct lpipe_lval v = {
        .type = TYPE_CALL,
        .gc_flag = 0,
        .inner.call = call
    }
    return lpipe_alloc_raw(ctx, v);
}

/*

Builtins:

(_lam (args) body)
(_if cond true false)
(_lt a b)
(_gt a b)
(_lte a b)
(_gte a b)
(_add a b)
(_sub a b)
(_car l)
(_cdr l)
(_sidx s i)
(_read s)
(_eval

*/
