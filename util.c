#include "nate.h"

void
die(const char *fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	fprintf(stderr, "die: ");
	vfprintf(stderr, fmt, vl);
	fprintf(stderr, "\n");
	va_end(vl);
	exit(1);
}

void*
emalloc(size_t sz)
{
	void *p = malloc(sz);
	if (p == NULL)
		die("out of memory");
	return p;	
}

char*
estrdup(char *s)
{
	char *n = malloc(strlen(s));
	memcpy(n, s, strlen(s));
	return n;
}
