#include "nate.h"

typedef struct node_t node_t;
typedef struct leaf_t leaf_t;
typedef struct buff_t buff_t;

struct node_t {
	size_t  weight;
	rope_t *lhs;
	rope_t *rhs;
};

struct leaf_t {
	buff_t *buff;
	size_t  start;
	size_t  end;
};

struct buff_t {
	char   *data;
	size_t  len;
};

struct rope_t {
	enum {
		ROPE_LEAF,
		ROPE_NODE
	} tag;
	union {
		leaf_t leaf;
		node_t node;
	} u;
};

static buff_t empty_buff = { "", 0 };
static rope_t empty_rope = { ROPE_LEAF, { { &empty_buff, 0, 0 } } };

static rope_t*
make_node(rope_t *lhs, rope_t *rhs, size_t weight)
{
	rope_t *r;
       
	assert(lhs);
	assert(rhs);

	r = emalloc(sizeof(rope_t));
	r->tag = ROPE_NODE;
	r->u.node.lhs = lhs;
	r->u.node.rhs = rhs;
	r->u.node.weight = weight;
	return r;
}

static rope_t*
make_leaf(buff_t *buff, size_t start, size_t end)
{
	rope_t *r;
       
	assert(buff);
	assert(start <= end);
	assert(start < buff->len);
	assert(end <= buff->len);

	r = emalloc(sizeof(rope_t));
	r->tag = ROPE_LEAF;
	r->u.leaf.buff = buff;
	r->u.leaf.start = start;
	r->u.leaf.end = end;
	return r;
}

static buff_t*
make_buff(char *data, size_t len)
{
	buff_t *b;

	assert(data);

	b = emalloc(sizeof(buff_t));
	b->data = estrdup(data);
	b->len = len;
	return b;
}

static size_t
rope_weight(rope_t *r)
{
	assert(r);

	switch (r->tag) {
	case ROPE_NODE:
		return r->u.node.weight;
	case ROPE_LEAF:
		return r->u.leaf.end - r->u.leaf.start;
	}
	assert(!"unreachable");
	return 0;
}

size_t
rope_len(rope_t *r)
{
	assert(r);

	switch (r->tag) {
	case ROPE_NODE:
		return rope_weight(r) + rope_len(r->u.node.rhs);
	case ROPE_LEAF:
		return rope_weight(r);
	}
	assert(!"unreachable");
	return 0;
}

rope_t*
rope_join(rope_t *lhs, rope_t *rhs)
{
	if (lhs == &empty_rope)
		return rhs;
	if (rhs == &empty_rope)
		return lhs;
	return make_node(lhs, rhs, rope_len(lhs));
}

rope_t*
rope_new()
{
	return &empty_rope;
}

rope_t*
rope_init(char *str, size_t len)
{
	buff_t *b = make_buff(str, len);	
	return make_leaf(b, 0, len);
}

static void
rope_split_node(rope_t **lhs, rope_t **rhs, rope_t *r, size_t off)
{
	size_t w;

	assert(lhs);
	assert(rhs);
	assert(r);
	assert(r->tag == ROPE_NODE);

	w = rope_weight(r);
	if (off < w) {
		rope_split(lhs, rhs, r->u.node.lhs, off);
		*rhs = rope_join(*rhs, r->u.node.rhs);
	} else {
		rope_split(lhs, rhs, r->u.node.rhs, off - w);
		*lhs = rope_join(r->u.node.lhs, *lhs);
	}
}

static void
rope_split_leaf(rope_t **lhs, rope_t **rhs, rope_t *r, size_t off)
{
	assert(lhs);
	assert(rhs);
	assert(r);
	assert(r->tag == ROPE_LEAF);

	if (off == 0) {
		*lhs = &empty_rope;
		*rhs = r;
	} else if (off >= rope_len(r)) {
		*lhs = r;
		*rhs = &empty_rope;
	} else {
		*lhs = make_leaf(r->u.leaf.buff, 
				 r->u.leaf.start,
				 off);
		*rhs = make_leaf(r->u.leaf.buff, 
				 r->u.leaf.start + off,
				 r->u.leaf.end);
	}
}

void
rope_split(rope_t **lhs, rope_t **rhs, rope_t *r, size_t off)
{
	assert(lhs);
	assert(rhs);
	assert(r);

	switch (r->tag) {
	case ROPE_NODE:
		rope_split_node(lhs, rhs, r, off);
		break;
	case ROPE_LEAF:
		rope_split_leaf(lhs, rhs, r, off);
		break;
	}
}

rope_t*
rope_insert(rope_t *r, size_t off, char *str, size_t len)
{
	rope_t *lhs, *rhs, *tmp;

	assert(r);
	assert(str);

	rope_split(&lhs, &rhs, r, off);	
	tmp = rope_join(lhs, rope_init(str, len));
	tmp = rope_join(tmp, rhs);
	return tmp;
}

static size_t
rope_read_node(rope_t *r, size_t off, char *buf, size_t len)
{
	size_t w, rd = 0;

	assert(r);
	assert(buf);
	assert(r->tag == ROPE_NODE);

	w = rope_weight(r);
	if (off < w) {
		rd += rope_read(r->u.node.lhs, off, buf, len);
		/* adjust arguments to account for what was read. */
		off += rd;
		buf += rd;
		len -= rd;
	}
	/* note: this isn't an 'else' statement. The inputs could have
	   been adjusted in the if statement above. */
	if (off >= w) {
		rd += rope_read(r->u.node.rhs, off - w, buf, len);
	}
	return rd;
}

static size_t
rope_read_leaf(rope_t *r, size_t off, char *buf, size_t len)
{
	size_t s, e;

	assert(r);
	assert(buf);
	assert(r->tag == ROPE_LEAF);

	s = MIN(r->u.leaf.end, r->u.leaf.start + off);
	e = MIN(r->u.leaf.end, s + len);
	memcpy(buf, r->u.leaf.buff->data + s, e - s);
	return e - s;
}

size_t
rope_read(rope_t *r, size_t off, char *buf, size_t len)
{
	switch (r->tag) {
	case ROPE_NODE:
		return rope_read_node(r, off, buf, len);
	case ROPE_LEAF:
		return rope_read_leaf(r, off, buf, len);
	}
	assert(!"unreachable");
	return 0;
}

void
rope_debug(rope_t *r)
{
	char buf[1024];

	switch (r->tag) {
	case ROPE_NODE:
		rope_debug(r->u.node.lhs);
		printf("node %p: %ld %p %p\n",
				(void*)r,
				r->u.node.weight,
				(void*)r->u.node.lhs,
				(void*)r->u.node.rhs);
		rope_debug(r->u.node.rhs);
		break;
	case ROPE_LEAF:
		assert(r->u.leaf.buff->len < 1023);
		snprintf(buf, r->u.leaf.buff->len + 1,
				"%s", r->u.leaf.buff->data);
		printf("leaf %p: %ld %ld (len=%ld, buf='%s')\n",
				(void*)r,
				r->u.leaf.start,
				r->u.leaf.end,
				r->u.leaf.buff->len,
				buf);
		break;
	}
}
