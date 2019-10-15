#include "nate.h"

rope_t	*rope;
size_t 	 cursor;

static void
init()
{
	rope = rope_new();
	cursor = 0;
}

static size_t
nextline(char *line, size_t len)
{
	char *s;
	int c;

	s = line;
	c = fgetc(stdin);
	if (c == EOF)
		return -1;
	while (c != '\n' && c != EOF) {
		*s++ = c;
		if ((s - line) >= len - 2)
			die("line too long");
		c = fgetc(stdin);
	}
	*s++ = '\n';
	*s = '\0';
	return s - line;
}

static void
append()
{
	size_t len;
	char line[1024];

	for (;;) {
		len = nextline(line, 1024);
		if (strcmp(line, ".\n") == 0)
			break;
		rope = rope_insert(rope, cursor, line, len);
		cursor += len;
	}
}

static void
print()
{
	char buf[1024];
	size_t rd, off = 0;

	while ((rd = rope_read(rope, off, buf, 1024)) != 0) {
		write(1, buf, rd);
		off += rd;
	}
}

void
shell()
{
	size_t len;
	char line[1024];

	init();

	for (;;) {
		len = nextline(line, 1024);
		if (len == -1)
			break;
		if (strcmp(line, "p\n") == 0)
			print();
		else if (strcmp(line, "a\n") == 0)
			append();
		else if (strcmp(line, "q\n") == 0)
			break;
		else
			printf("?\n");
	}
}
