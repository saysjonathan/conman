#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conman.h"
#include "util.h"

static char *type = "[pkg]";
static char *pkg = NULL;
static int state = CM_PRESENT;
static int noop = 0;
static int verbose = 0;

void run(void) {
	int i;
	cm_pkg *p = malloc(sizeof(cm_pkg));
	cm_pkg_init(p);
	p->state = state;
	i = cm_pkg_set(pkg, p);
	cm_pkg_deinit(p);
	free(p);
	if(i != 0) {
		die("%s %s: Unable to manage package: %s\n", type, pkg, cm_strerror(i));
	}
	return;
}

void diff(void) {
	int i;
	cm_pkg *p = malloc(sizeof(cm_pkg));
	cm_pkg_init(p);
	i = cm_pkg_get(pkg, p);
	if(i != 0) {
		cm_pkg_deinit(p);
		free(p);
		die("%s %s: Unable to get package info: %s\n", type, pkg, cm_strerror(i));
	}
	i = diff_state(state, p->state, type, pkg);
	if(i != 0) {
		goto cleanup;
	}
cleanup:
	cm_pkg_deinit(p);
	free(p);
	return;
}

void usage(void) {
	fprintf(stderr, "usage: cm-pkg [options] PACKAGE\n"
			"\n"
			"Options:\n"
			"\t-a, --absent         ensure package is uninstalled\n"
			"\t-h, --help           display this message and exit\n"
			"\t-n, --noop           print but do not commit changes\n"
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
		{"help", no_argument, NULL, 'h'},
		{"latest", no_argument, NULL, 'l'},
		{"noop", no_argument, NULL, 'n'},
		{"verbose", no_argument, NULL, 'v'},
		{NULL, 0, NULL, '\0'}
	};

	while((i = getopt_long(argc, argv, "ahnv", lopts, NULL)) != -1) {
		switch(i) {
			case 'a':
				state = CM_ABSENT;
				break;
			case 'n':
				noop = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
			default:
				usage();
		}
	}
	pkg = argv[optind];
	if(pkg == NULL) {
		usage();
	}
	if(verbose||noop) {
		diff();
	}
	if(!noop) {
		run();
	}
	exit(EXIT_SUCCESS);
}
