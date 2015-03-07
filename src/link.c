#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conman.h"
#include "util.h"

static char *type = "[link]";
static char *path = NULL;
static char *owner = NULL;
static char *group = NULL;
static int state = CM_PRESENT;
static char *target = NULL;
static int noop = 0;
static int verbose = 0;

void run(void) {
	int i;
	cm_link *l = malloc(sizeof(cm_link));
	cm_link_init(l);
	l->state = state;
	if(state == CM_PRESENT) {
		strcpy(l->target, target);
	}
	if(owner) {
		strcpy(l->owner, owner);
	}
	if(group) {
		strcpy(l->group, group);
	}
	i = cm_link_set(path, l);
	cm_link_deinit(l);
	free(l);
	if(i != 0) {
		die("%s %s: Unable to manage link: %s\n", type, path, cm_strerror(i));
	}
	return;
}

void diff(void) {
	int i;
	cm_link *l = malloc(sizeof(cm_link));
	cm_link_init(l);
	i = cm_link_get(path, l);
	if(i != 0) {
		cm_link_deinit(l);
		free(l);
		die("%s %s: Unable to get link info: %s\n", type, path, cm_strerror(i));
	}
	i = diff_state(state, l->state, type, path);
	if(i != 0) {
		goto cleanup;
	}
	if(state == CM_PRESENT) {
		diff_string(target, l->target, type, "target", path);
		if(owner) {
			diff_string(owner, l->owner, type, "owner", path);
		}
		if(group) {
			diff_string(group, l->group, type, "group", path);
		}
	}
	goto cleanup;
cleanup:
	cm_link_deinit(l);	
	free(l);
	return;
}

void usage(void) {
	fprintf(stderr, "usage: cm-link [options] -t TARGET PATH\n"
			"\n"
			"Options:\n"
			"\t-a, --absent         ensure link is absent\n"
			"\t-g, --group GROUP	link group\n"
			"\t-h, --help           display this message and exit\n"
			"\t-n, --noop           print but do not commit changes\n"
			"\n-o, --owner OWNER	link owner\n"
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
		{"noop", no_argument, NULL, 'n'},
		{"target", required_argument, NULL, 't'},
		{"verbose", no_argument, NULL, 'v'},
		{NULL, 0, NULL, '\0'}
	};

	while((i = getopt_long(argc, argv, "ag:hno:t:v", lopts, NULL)) != -1) {
		switch(i) {
			case 'a':
				state = CM_ABSENT;
				break;
			case 'g':
				group = optarg;
				break;
			case 'n':
				noop = 1;
				break;
			case 'o':
				owner = optarg;
				break;
			case 't':
				target = optarg;
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
	if(state == CM_PRESENT && !target) {
		die("Target must be specified: See usage\n");
	}
	if(verbose||noop) {
		diff();
	}
	if(!noop) {
		run();
	}
	exit(EXIT_SUCCESS);
}
