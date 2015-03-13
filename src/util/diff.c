#include <string.h>
#include "conman.h"
#include "../util.h"

int diff_state(int s, int r, char *type, char *name) {
	if(cm_diff_state(r, s) != 0) {
		char *sn = state_string(s);
		char *rn = state_string(r);
		logmsg("%s %s: is %s and should be %s\n", type, name, rn, sn);
		return 1;
	}
	return 0;
}

char *state_string(int s) {
	char *n;
	switch(s) {
		case CM_ABSENT:
			n = "absent";
			break;
		case CM_PRESENT:
			n = "present";
			break;
		case CM_OUTDATED:
			n = "outdated";
			break;
		case CM_LATEST:
			n = "latest";
			break;
	}
	return n;
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
