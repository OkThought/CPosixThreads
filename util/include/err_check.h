#pragma once

#define NO_CLEANUP NULL
#define NO_ARG NULL
#define NO_MESSAGE NULL
#define NO_ERROR 0

void ExitIfTrueWithErrcodeAndCleanupAndFormattedMessage(int cond, int code, void (*cleanup)(void *), void *arg,
                                                        const char *fmt, ...);
void ExitIfTrueWithErrcodeAndCleanupAndMessage(int cond, int code, void (*cleanup)(void *), void *arg, const char *msg);
void ExitIfTrueWithErrcodeAndCleanup(int cond, int code, void (*cleanup)(void *), void *arg);
void ExitIfTrueWithErrcodeAndFormattedMessage(int cond, int code, const char *fmt, ...);
void ExitIfTrueWithErrcodeAndMessage(int cond, int code, const char *msg);

void ExitIfNonZeroWithCleanupAndMessage(int code, void (*cleanup)(void *), void *arg, const char *msg);
void ExitIfNonZeroWithCleanupAndFormattedMessage(int code, void (*cleanup)(void *), void *arg, const char *fmt, ...);
void ExitIfNonZeroWithCleanup(int code, void (*cleanup)(void *), void *arg);
void ExitIfNonZeroWithMessage(int code, char *msg);
void ExitIfNonZeroWithFormattedMessage(int code, const char *fmt, ...);
void ExitIfNonZero(int code);

void ExitIfNullWithCleanupAndMessage(void *ptr, void (*cleanup)(void *), void *arg, const char *msg);
void ExitIfNullWithCleanupAndFormattedMessage(void *ptr, void (*cleanup)(void *), void *arg, const char *fmt, ...);
void ExitIfNullWithCleanup(void *ptr, void (*cleanup)(void *), void *arg);
void ExitIfNullWithMessage(void *ptr, const char *msg);
void ExitIfNullWithFormattedMessage(void *ptr, const char *fmt, ...);
void ExitIfNull(void *ptr);
