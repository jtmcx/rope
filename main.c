#include "nate.h"

void
test(rope_t *r)
{
	char buf[1024];
	size_t rd, off = 0;

	rd = rope_read(r, off, buf, 2);
	assert(rd == 2);
	printf("%2s", buf);
	assert(buf[0] == 'h');
	assert(buf[1] == 'e');
	off += 2;

	rd = rope_read(r, off, buf, 2);
	assert(rd == 2);
	printf("%2s", buf);
	assert(buf[0] == 'l');
	assert(buf[1] == 'l');
	off += 2;

	rd = rope_read(r, off, buf, 2);
	assert(rd == 2);
	printf("%2s", buf);
	assert(buf[0] == 'o');
	assert(buf[1] == 'w');
	off += 2;

	rd = rope_read(r, off, buf, 2);
	assert(rd == 2);
	printf("%2s", buf);
	assert(buf[0] == 'o');
	assert(buf[1] == 'r');
	off += 2;

	rd = rope_read(r, off, buf, 2);
	assert(rd == 2);
	printf("%2s", buf);
	assert(buf[0] == 'l');
	assert(buf[1] == 'd');
	off += 2;

	printf("\n");
}

int
main(int argc, char *argv[])
{
	rope_t *r = rope_new();
	r = rope_insert(r, 0, "helld", 5);
	r = rope_insert(r, 3, "lowor", 5);
	rope_debug(r);
	test(r);
	return 0;
}
