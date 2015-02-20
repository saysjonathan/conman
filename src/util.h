#ifndef _CM_BIN_UTIL
#define _CM_BIN_UTIL

void die(const char *errstr, ...);
void logerr(const char *errstr, ...);
void logmsg(const char *errstr, ...);

int diff_state(int s, int r, char *type, char *name);
void diff_string(char *o, char *n, char *type, char *field, char *name);
void diff_int(int o, int n, char *type, char *field, char *name);
#endif
