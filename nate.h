#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/* rope.c */
struct rope_t;
typedef struct rope_t rope_t;

rope_t* rope_join(rope_t *lhs, rope_t *rhs);
rope_t* rope_new();
rope_t* rope_init(char *s, size_t len);
void    rope_split(rope_t **lhs, rope_t **rhs, rope_t *r, size_t off);
rope_t* rope_insert(rope_t *r, size_t off, char *str, size_t len);
size_t  rope_read(rope_t *r, size_t off, char *buf, size_t len);
void    rope_debug(rope_t *r);

/* util.c */
void*   emalloc(size_t);
void    die(const char *fmt, ...);
char*   estrdup(char *s);
