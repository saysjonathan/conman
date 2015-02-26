#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conman/conman.h"
#include "util.h"

static char *type = "[user]";
static char *user = NULL;
static char *passwd = NULL;
static char *group = NULL;
static char *comment = NULL;
static char *homedir = NULL;
static char *shell = NULL;
static int uid = 0;
static int state = CM_PRESENT;
static int noop = 0;
static int verbose = 0;

void run(void) {
	int i;
	cm_user *u = malloc(sizeof(cm_user));
	cm_user_init(u);
	u->state = state;
	if(passwd) {
		strcpy(u->passwd, passwd);
	}
	if(group) {
		strcpy(u->group, group);
	}
	if(uid) {
		u->uid = uid;
	}
	if(comment) {
		strcpy(u->comment, comment);
	}
	if(homedir) {
		strcpy(u->homedir, homedir);
	}
	if(shell) {
		strcpy(u->shell, shell);
	}
	i = cm_user_set(user, u);
	cm_user_deinit(u);
	free(u);
	if(i != 0) {
		die("%s %s: Unable to manage user: %s\n", type, user, cm_strerror(i));
	}
	return;
}

void diff(void) {
	int i;
	cm_user *u = malloc(sizeof(cm_user));
	cm_user_init(u);
	i = cm_user_get(user, u);
	if(i != 0) {
		cm_user_deinit(u);
		free(u);
		die("%s %s: Unable to get user info: %s\n", type, user, cm_strerror(i));
	}
	i = diff_state(state, u->state, type, user);
	if(i != 0) {
		goto cleanup;
	}
	if(state == CM_PRESENT) {
		if(passwd) {
			diff_string(passwd, u->passwd, type, "passwd", user);
		}
		if(uid) {
			diff_int(uid, u->uid, type, "uid", user);
		}
		if(group) {
			diff_string(group, u->group, type, "group", user);
		}
		if(comment) {
			diff_string(comment, u->comment, type, "comment", user);
		}
		if(homedir) {
			diff_string(homedir, u->homedir, type, "homedir", user);
		}
		if(shell) {
			diff_string(shell, u->shell, type, "shell", user);
		}
	}
	goto cleanup;
cleanup:
	cm_user_deinit(u);
	free(u);
	return;
}

void usage(void) {
	fprintf(stderr, "usage: cm-user [options] -g GROUP -d HOMEDIR -s SHELL USER\n"
			"\n"
			"Options:\n"
			"\t-a, --absent           ensure user is absent\n"
			"\t-c, --comment COMMENT  user comment\n"
			"\t-d, --dir HOMEDIR      user home directory\n"
			"\t-g, --group GROUP      primary user group\n"
			"\t-h, --help             display this message and exit\n"
			"\t-n, --noop             print but do not commit changes\n"
			"\t-p, --pass PASSWD      encrypted user password\n"
			"\t-s, --shell SHELL      user shell\n"
			"\t-u, --uid UID          user uid\n"
			"\t-v, --verbose          enable verbose output\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
	int i;
	if(argc <= 1) {
		usage();
	}

	static struct option lopts[] = {
		{"absent", no_argument, NULL, 'a'},
		{"comment", required_argument, NULL, 'c'},
		{"dir", required_argument, NULL, 'd'},
		{"group", required_argument, NULL, 'g'},
		{"help", no_argument, NULL, 'h'},
		{"noop", no_argument, NULL, 'n'},
		{"pass", required_argument, NULL, 'p'},
		{"shell", required_argument, NULL, 's'},
		{"uid", required_argument, NULL, 'u'},
		{"verbose", no_argument, NULL, 'v'},
		{NULL, 0, NULL, '\0'}
	};

	while((i = getopt_long(argc, argv, "ac:d:g:hnp:s:u:v", lopts, NULL)) != -1) {
		switch(i) {
			case 'a':
				state = CM_ABSENT;
				break;
			case 'c':
				comment = optarg;
				break;
			case 'd':
				homedir = optarg;
				break;
			case 'g':
				group = optarg;
				break;
			case 'n':
				noop = 1;
				break;
			case 'p':
				passwd = optarg;
				break;
			case 's':
				shell = optarg;
				break;
			case 'u':
				uid = atoi(optarg);
				break;
			case 'v':
				verbose = 1;
				break;
			case 'h':
			default:
				usage();
		}
	}
	user = argv[optind];
	if(!user) {
		usage();
	}
	if((state != CM_ABSENT) && (!group || !shell || !homedir)) {
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
