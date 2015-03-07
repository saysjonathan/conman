#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conman.h"
#include "util.h"

static char *type = "[dir]";
static char *path = NULL;
static int state = CM_PRESENT;
static char *owner = NULL;
static char *group = NULL;
static char *mode = NULL;
static int recurse = 0;
static int noop = 0;
static int verbose = 0;

void run(void) {
	int i;
	cm_dir *d = malloc(sizeof(cm_dir));
	cm_dir_init(d);
	d->state = state;
	d->recurse = recurse;
	if(owner) {
		strcpy(d->owner, owner);
	}
	if(group) {
		strcpy(d->group, group);
	}
	if(mode) {
		strcpy(d->mode, mode);
	}
	i = cm_dir_set(path, d);
	cm_dir_deinit(d);
	free(d);
	if(i != 0) {
		die("%s %s: Unable to manage file: %s\n", type, path, cm_strerror(i));
	}
	return;
}

void diff(void) {
	int i;
	cm_dir *d = malloc(sizeof(cm_dir));
	cm_dir_init(d);
	i = cm_dir_get(path, d);
	if(i != 0) {
		free(d);
		die("%s %s: Unable to get dir info: %s\n", type, path, cm_strerror(i));
	}
	i = diff_state(state, d->state, type, path);
	if(i != 0) {
		goto cleanup;
	}
	if(state == CM_PRESENT) {
		if(owner) {
			diff_string(owner, d->owner, type, "owner", path);
		}
		if(group) {
			diff_string(group, d->group, type, "group", path);
		}
		if(mode) {
			diff_string(mode, d->mode, type, "mode", path);
		}
	}
cleanup:
	cm_dir_deinit(d);
	free(d);
	return;
}

void usage(void) {
	fprintf(stderr, "usage: cm-dir [options] PATH\n"
			"\n"
			"Options:\n"
			"\t-a, --absent         ensure dir is absent\n"
			"\t-g, --group GROUP    dir group\n"
			"\t-h, --help           display this message and exit\n"
			"\t-m, --mode MODE      dir mode (4 digit octal)\n"
			"\t-n, --noop           print but do not commit changes\n"
			"\t-o, --owner OWNER    dir owner\n"
			"\t-r, --recurse        recursively manage directory contents\n"
			"\t-v, --verbose        enable verbose output\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	int i;
	if(argc <= 1) {
		usage();
	}

	static struct option lopts[] = {
		{"absent", no_argument, NULL, 'a'},
		{"group", required_argument, NULL, 'g'},
		{"help", no_argument, NULL, 'h'},
		{"mode", required_argument, NULL, 'm'},
		{"noop", no_argument, NULL, 'n'},
		{"owner", required_argument, NULL, 'o'},
		{"recurse", no_argument, NULL, 'r'},
		{"verbose", no_argument, NULL, 'v'},
		{NULL, 0, NULL, '\0'}
	};

	while((i = getopt_long(argc, argv, "ag:hm:no:rt:v", lopts, NULL)) != -1) {
		switch(i) {
			case 'a':
				state = CM_ABSENT;
				break;
			case 'g':
				group = optarg;
				break;
			case 'm':
				mode = optarg;
				break;
			case 'n':
				noop = 1;
				break;
			case 'o':
				owner = optarg;
				break;
			case 'r':
				recurse = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
			default:
				usage();
		}
	}
	path = argv[optind];
	if(verbose||noop) {
		diff();
	}
	if(!noop) {
		run();
	}
	exit(EXIT_SUCCESS);
}
