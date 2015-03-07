#include <string.h>
#include "conman.h"
#include "../util.h"

int diff_state(int s, int r, char *type, char *name) {
	if(s == r) {
		return 0;
	}
	if(r == CM_ABSENT) {
		logmsg("%s %s: is absent and should be present\n", type, name);
		return 1;
	}
	if(r == CM_PRESENT) {
		logmsg("%s %s: is present and should be absent\n", type, name);
		return 1;
	}
	return 0;
}

void diff_string(char *o, char *n, char *type, char *field, char *name) {
	int i = strcmp(o, n);
	if(i != 0) {
		logmsg("%s %s: %s is '%s' and should be '%s'\n", type, name, field, n, o);
	}
}

void diff_int(int o, int n, char *type, char *field, char *name) {
	if(o != n) {
		logmsg("%s %s: %s is '%d' and should be '%d'\n", type, name, field, n, o);
	}
}
