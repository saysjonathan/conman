#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conman.h"
#include "util.h"

static char *type = "[file]";
static char *path = NULL;
static int state = CM_PRESENT;
static char *owner = NULL;
static char *group = NULL;
static char *mode = NULL;
static char *source = NULL;
static int noop = 0;
static int verbose = 0;

static cm_file *setup(void) {
	cm_file *f = malloc(sizeof(cm_file));
	cm_file_init(f);
	return f;
}

static void teardown(cm_file *f) {
	cm_file_deinit(f);
	free(f);
}

static void get(char *file, cm_file *f) {
	int i;
	i = cm_file_get(file, f);
	if(i != 0) {
		teardown(f);
		die("%s %s: Unable to get file info: %s\n", type, file, cm_strerror(i));
	}
}

void run(void) {
	int i;
	cm_file *f = setup();
	f->state = state;
	if(owner) {
		strcpy(f->owner, owner);
	}
	if(group) {
		strcpy(f->group, group);
	}
	if(mode) {
		strcpy(f->mode, mode);
	}
	if(source) {
		strcpy(f->source, source);
	}
	i = cm_file_set(path, f);
	teardown(f);
	if(i != 0) {
		die("%s %s: Unable to manage file: %s\n", type, path, cm_strerror(i));
	}
	return;
}

void diff(void) {
	int i;
	cm_file *f = setup();
	get(path, f);
	i = diff_state(state, f->state, type, path);
	if(i != 0) {
		teardown(f);
		return;
	}
	if(state == CM_PRESENT) {
		if(owner) {
			diff_string(owner, f->owner, type, "owner", path);
		}
		if(group) {
			diff_string(group, f->group, type, "group", path);
		}
		if(mode) {
			diff_string(mode, f->mode, type, "mode", path);
		}
		if(source) {
			cm_file *s = setup();
			get(source, s);
			diff_string(f->hash, s->hash, type, "content hash", path);
			teardown(s);
		}
			
	}
	teardown(f);
	return;
}

void usage(void) {
	fprintf(stderr, "usage: cm-file [options] PATH\n"
			"\n"
			"Options:\n"
			"\t-a, --absent         ensure file is absent\n"
			"\t-g, --group GROUP    file group\n"
			"\t-h, --help           display this message and exit\n"
			"\t-m, --mode MODE      file mode (4 digit octal)\n"
			"\t-n, --noop           print but do not commit changes\n"
			"\t-o, --owner OWNER    file owner\n"
			"\t-s, --source PATH    source file path\n"
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
		{"source", required_argument, NULL, 's'},
		{"verbose", no_argument, NULL, 'v'},
		{NULL, 0, NULL, '\0'}
	};

	while((i = getopt_long(argc, argv, "ag:hm:no:s:v", lopts, NULL)) != -1) {
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
			case 's':
				source = optarg;
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
