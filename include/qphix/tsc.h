#pragma once

// Stuff for rdtsc
#include <sys/types.h>
#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) ||           \
    defined(__OpenBSD__)
#include <sys/sysctl.h>
#endif

typedef long long TSC_tick;
#define CLOCK_NOW(a) (a) = __rdtsc()
