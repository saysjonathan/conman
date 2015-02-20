#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "../util.h"

void die(const char *errstr, ...) {
	va_list a;
	va_start(a, errstr);
	vfprintf(stderr, errstr, a);
	va_end(a);
	exit(EXIT_FAILURE);
}

void logerr(const char *errstr, ...) {
	va_list a;
	va_start(a, errstr);
	vfprintf(stderr, errstr, a);
	va_end(a);
}

void logmsg(const char *errstr, ...) {
	va_list a;
	va_start(a, errstr);
	vfprintf(stdout, errstr, a);
	va_end(a);
}
