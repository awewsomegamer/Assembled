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

#include <includes.h>
#include <interface/interface.h>
#include <editor/functions.h>
#include <editor/buffer/editor.h>

#define TARGET_FPS (double)(1.0/60.0)

#define AS_COLOR_HIGHLIGHT 1 

#ifdef DEBUG_MODE
#define AS_DEBUG_CODE(code) code
#define AS_DEBUG_MSG(...) { \
			for (int __i__ = fprintf(__AS_DBG_LOG_FILE__, "[%s:%d, AS]: ", __FILE_NAME__, __LINE__); __i__ < 24; __i__++) \
				fputc(' ', __AS_DBG_LOG_FILE__); \
			fprintf(__AS_DBG_LOG_FILE__, __VA_ARGS__); \
		       }
#else
#define AS_DEBUG_CODE(code)
#define AS_DEBUG_MSG(...)
#endif

#define max(a, b) \
	({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a > _b ? _a : _b; })


#define min(a, b) \
	({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a < _b ? _a : _b; })

struct AS_GlobalCtx {
	// Editor Screen
	char editor_scr_message[1024];
	
	// Interface
	struct AS_RenderCtx render_ctx;
	struct AS_Screen screens[MAX_SCREEN_COUNT];
	struct AS_Screen *screen;
	
	// Functions
	void (*functions[MAX_FUNCTION_COUNT])();

	// Editor
	struct AS_TextFile *text_files[MAX_TEXT_FILES];
	struct AS_TextFile *text_file;
	int text_file_i;

	struct AS_ColDesc col_descs[MAX_COLUMNS];
	int col_desc_i;
};

extern struct AS_GlobalCtx as_ctx;

#endif