/**
 * @file main.c
 * @author awewsomegamer <awewsomegamer@gmail.com>
 *
 * @section LICENSE
 *
 * Assembled - Column based text editor
 * Copyright (C) 2023-2024 awewsomegamer
 *
 * This file is apart of Assembled.
 *
 * Assembled is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @section DESCRIPTION
 *
 * The main program file
*/

#ifndef AS_INCLUDES_H
#define AS_INCLUDES_H

#include <bits/time.h>
#include <sys/stat.h>

#include <curses.h>
#include <ncurses.h>
#include <curses.h>
#include <pwd.h>
#include <locale.h>
#include <signal.h>
#include <time.h>
#include <math.h>
#include <dirent.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>

#include <string.h>

#ifdef DEBUG_MODE
#define AS_DEBUG_CODE(code) code
#define AS_DEBUG_MSG(...) { \
			for (int __i__ = fprintf(__AS_DBG_LOG_FILE__, "[%s:%d, AS]: ", __FILE_NAME__, __LINE__); __i__ < 40; __i__++) \
				fputc(' ', __AS_DBG_LOG_FILE__); \
			fprintf(__AS_DBG_LOG_FILE__, __VA_ARGS__); \
		       }
#else
#define AS_DEBUG_CODE(code)
#define AS_DEBUG_MSG(...)
#endif

extern FILE *__AS_DBG_LOG_FILE__;

#endif
