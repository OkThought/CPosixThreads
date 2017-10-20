#pragma once

#define NO_CLEANUP NULL
#define NO_ARG NULL

void ExitIfNonZeroWithCleanupAndMessage(int code, void (*cleanup)(void *), void *arg, char *msg);
void ExitIfNonZeroWithMessage(int code, char *msg);
void ExitIfNonZero(int code);
void ExitIfNullWithCleanupAndMessage(void *ptr, void (*cleanup)(void *), void *arg, char *msg);
void ExitIfNullWithMessage(void *ptr, char *msg);
void ExitIfNull(void *ptr);
