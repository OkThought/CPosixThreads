#include "err_check.h"

#include <stdio.h> // fprintf
#include <stdlib.h> // exit
#include <string.h> // strerror

void ExitIfNonZeroWithCleanupAndMessage(int code, void (*cleanup)(void *), void *arg, char *msg) {
	if (code == 0) return;

	if (cleanup != NO_CLEANUP) {
		cleanup(arg);
	}
	if (msg != NULL) {
	  fprintf(stderr, "%s: ", msg);
	}
	fputs(strerror(code), stderr);
	exit(EXIT_FAILURE);
}

void ExitIfNonZeroWithMessage(int code, char *msg) {
	ExitIfNonZeroWithCleanupAndMessage(code, NO_CLEANUP, NO_ARG, msg);
}

void ExitIfNonZero(int code) {
	if (code != 0) {
		exit(EXIT_FAILURE);
	}
}

void ExitIfNullWithCleanupAndMessage(void *p, void (*cleanup)(void *), void *arg, char *msg) {
	if (p == NULL) {
		if (cleanup != NO_CLEANUP) {
			cleanup(arg);
		}
		if (msg != NULL) {
			fputs(msg, stderr);
		}
		exit(EXIT_FAILURE);
	}
}

void ExitIfNullWithMessage(void *p, char *msg) {
	ExitIfNullWithCleanupAndMessage(p, NO_CLEANUP, NO_ARG, msg);
}

void ExitIfNull(void *p) {
	if (p == NULL) {
		exit(EXIT_FAILURE);
	}
}
