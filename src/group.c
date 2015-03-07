#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conman.h"
#include "util.h"

static char *type = "[group]";
static char *group = NULL;
static int gid = 0;
static int state = CM_PRESENT;
static char *passwd = NULL;
static int noop = 0;
static int verbose = 0;

void run(void) {
	int i;
	cm_group *g = malloc(sizeof(cm_group));
	cm_group_init(g);
	g->state = state;
	if(gid) {
		g->gid = gid;
	}
	if(passwd) {
		strcpy(g->passwd, passwd);
	}
	i = cm_group_set(group, g);
	cm_group_deinit(g);
	free(g);
	if(i != 0) {
		die("%s %s: Unable to manage group: %s\n", type, group, cm_strerror(i));
	}
	return;
}

void diff(void) {
	int i;
	cm_group *g = malloc(sizeof(cm_group));
	cm_group_init(g);
	i = cm_group_get(group, g);
	if(i != 0) {
		cm_group_deinit(g);
		free(g);
		die("%s %s: Unable to get group info: %s\n", type, group, cm_strerror(i));
	}
	i = diff_state(state, g->state, type, group);
	if(i != 0) {
		goto cleanup;
	}
	if(state == CM_PRESENT) {
		if(gid) {
			diff_int(gid, g->gid, type, "gid", group);
		}
		if(passwd) {
			diff_string(passwd, g->passwd, type, "passwd", group);
		}
	}
	goto cleanup;
cleanup:
	cm_group_deinit(g);
	free(g);
	return;
}

void usage(void) {
	fprintf(stderr, "usage: cm-group [options] GROUP\n"
			"\n"
			"Options:\n"
			"\t-a, --absent         ensure group is absent\n"
			"\t-g, --gid GID        group gid\n"
			"\t-h, --help           display this message and exit\n"
			"\t-n, --noop           print but do not commit changes\n"
			"\t-p, --pass           encrypted group pass\n"
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
		{"gid", required_argument, NULL, 'g'},
		{"help", no_argument, NULL, 'h'},
		{"noop", no_argument, NULL, 'n'},
		{"pass", required_argument, NULL, 'n'},
		{"verbose", no_argument, NULL, 'v'},
		{NULL, 0, NULL, '\0'}
	};

	while((i = getopt_long(argc, argv, "ag:hnp:v", lopts, NULL)) != -1) {
		switch(i) {
			case 'a':
				state = CM_ABSENT;
				break;
			case 'g':
				gid = atoi(optarg);
				break;
			case 'n':
				noop = 1;
				break;
			case 'p':
				passwd = optarg;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
			default:
				usage();
		}
	}
	group = argv[optind];
	if(verbose||noop) {
		diff();
	}
	if(!noop) {
		run();
	}
	exit(EXIT_SUCCESS);
}
