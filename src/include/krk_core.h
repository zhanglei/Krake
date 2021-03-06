/**
 * krk_core.h - Krake core
 * 
 * Copyright (c) 2010 Yang Yang <paulyang.inf@gmail.com>
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef __KRK_CORE_H__
#define __KRK_CORE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#define PID_FILE "/var/run/krake.pid"

#define KRK_OK 0
#define KRK_ERROR -1
#define KRK_AGAIN -2
#define KRK_BUSY -3
#define KRK_DONE -4
#define KRK_AGAIN_READ -5
#define KRK_AGAIN_WRITE -6

#define KRK_NAME_LEN 64
#define KRK_IPADDR_LEN 64
#define KRK_ARG_LEN 32

extern char krk_config_file[];

#endif
