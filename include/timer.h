#ifndef RECCA_TIMER_H
#define RECCA_TIMER_H

#include "types.h"
#include <time.h>
#include "config.h"

void timer_init(Timer *t);
void timer_start(Timer *t);
void timer_end(Timer *t);
void timer_report(Timer *t, const char *module_name);

#endif