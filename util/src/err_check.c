#include "err_check.h"

#include <stdio.h> // fprintf
#include <stdlib.h> // exit
#include <string.h> // strerror
#include <stdarg.h>

static void
exit_cond_cleanup_fmt_v(int cond, int errcode, void (*cleanup)(void *), void *arg, const char *fmt, va_list list) {
    if (!cond)
        return;

    if (cleanup != NO_CLEANUP)
        cleanup(arg);

    if (fmt != NULL)
        vfprintf(stderr, fmt, list);

    if (errcode != NO_ERROR)
        fprintf(stderr, ": %s\n", strerror(errcode));

    exit(EXIT_FAILURE);
}

static void
exit_cond_cleanup_msg(int cond, int errcode, void (*cleanup)(void *), void *arg, const char *msg) {
    if (!cond)
        return;

    if (cleanup != NO_CLEANUP)
        cleanup(arg);

    if (msg != NO_MESSAGE)
        fputs(msg, stderr);

    if (errcode != NO_ERROR)
        fprintf(stderr, ": %s\n", strerror(errcode));

    exit(EXIT_FAILURE);
}

void
ExitIfTrueWithErrcodeAndCleanupAndFormattedMessage(int cond, int code, void (*cleanup)(void *), void *arg,
                                                   const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);
    exit_cond_cleanup_fmt_v(cond, code, cleanup, arg, fmt, list);
    va_end(list);
}

void
ExitIfTrueWithErrcodeAndCleanupAndMessage(int cond, int code, void (*cleanup)(void *), void *arg, const char *msg) {
    exit_cond_cleanup_msg(cond, code, cleanup, arg, msg);
}

void
ExitIfTrueWithErrcodeAndCleanup(int cond, int code, void (*cleanup)(void *), void *arg) {
    exit_cond_cleanup_msg(cond, code, cleanup, arg, NO_MESSAGE);
}

void
ExitIfTrueWithErrcodeAndFormattedMessage(int cond, int code, const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);
    exit_cond_cleanup_fmt_v(cond, code, NO_CLEANUP, NO_ARG, fmt, list);
    va_end(list);
}

void
ExitIfTrueWithErrcodeAndMessage(int cond, int code, const char *msg) {
    exit_cond_cleanup_msg(cond, code, NO_CLEANUP, NO_ARG, msg);
}

void
ExitIfNonZeroWithCleanupAndMessage(int code, void (*cleanup)(void *), void *arg, const char *msg) {
	exit_cond_cleanup_msg((code != 0), code, cleanup, arg, msg);
}

void
ExitIfNonZeroWithCleanupAndFormattedMessage(int code, void (*cleanup)(void *), void *arg, const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);
    exit_cond_cleanup_fmt_v((code != 0), code, cleanup, arg, fmt, list);
    va_end(list);
}

void
ExitIfNonZeroWithCleanup(int code, void (*cleanup)(void *), void *arg) {
    exit_cond_cleanup_msg((code != 0), code, cleanup, arg, NO_MESSAGE);
}

void
ExitIfNonZeroWithMessage(int code, char *msg) {
	exit_cond_cleanup_msg((code != 0), code, NO_CLEANUP, NO_ARG, msg);
}

void
ExitIfNonZeroWithFormattedMessage(int code, const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);
    exit_cond_cleanup_fmt_v((code != 0), code, NO_CLEANUP, NO_ARG, fmt, list);
    va_end(list);
}

void
ExitIfNonZero(int code) {
	if (code != 0)
		exit(EXIT_FAILURE);
}



void
ExitIfNullWithCleanupAndMessage(void *ptr, void (*cleanup)(void *), void *arg, const char *msg) {
	exit_cond_cleanup_msg((ptr == NULL), NO_ERROR, cleanup, arg, msg);
}

void
ExitIfNullWithCleanupAndFormattedMessage(void *ptr, void (*cleanup)(void *), void *arg, const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);
    exit_cond_cleanup_fmt_v((ptr == NULL), NO_ERROR, cleanup, arg, fmt, list);
    va_end(list);
}

void
ExitIfNullWithCleanup(void *ptr, void (*cleanup)(void *), void *arg) {
    exit_cond_cleanup_msg((ptr == NULL), NO_ERROR, cleanup, arg, NO_MESSAGE);
}

void
ExitIfNullWithMessage(void *ptr, const char *msg) {
	exit_cond_cleanup_msg((ptr == NULL), NO_ERROR, NO_CLEANUP, NO_ARG, msg);
}

void
ExitIfNullWithFormattedMessage(void *ptr, const char *fmt, ...) {
    va_list list;
    va_start(list, fmt);
    exit_cond_cleanup_fmt_v((ptr == NULL), NO_ERROR, NO_CLEANUP, NO_ARG, fmt, list);
    va_end(list);
}

void
ExitIfNull(void *ptr) {
	if (ptr == NULL) {
		exit(EXIT_FAILURE);
	}
}
