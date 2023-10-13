/*
*    Assembled - Column based text editor
*    Copyright (C) 2023 awewsomegamer
*
*    This file is apart of Assembled.
*
*    Assembled is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License
*    as published by the Free Software Foundation; version 2
*    of the License.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software
*    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <ncurses.h>

#define TARGET_FPS (double)(1.0/60.0)

#define ASSEMBLED_COLOR_HIGHLIGHT 1 

extern FILE *debug_log_file;

#ifdef DEBUG_MODE
#define DEBUG_CODE(code) code
#define DEBUG_MSG(...)  { \
                                for (int __i__ = fprintf(debug_log_file, "[%s:%d]: ", __FILE_NAME__, __LINE__); __i__ < 24; __i__++) \
                                   fputc(' ', debug_log_file); \
                                fprintf(debug_log_file, __VA_ARGS__); \
                        }
#else
#define DEBUG_CODE(code)
#define DEBUG_MSG(...)
#endif

 #define max(a, b) \
        ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a > _b ? _a : _b; })


 #define min(a, b) \
        ({ __typeof__ (a) _a = (a); \
        __typeof__ (b) _b = (b); \
        _a < _b ? _a : _b; })


#endif