/**
 * @file global.h
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
 * The global header file. This gives the editor and interface halves access to each
 * other through the variable as_ctx.
*/

#ifndef AS_GLOBAL_H
#define AS_GLOBAL_H

#include <editor/syntax/syntax.h>
#include <editor/buffer/editor.h>

#include <interface/interface.h>

#include <includes.h>

/// The number of frames to be drawn per second
#define TARGET_FPS (double)(1.0/60.0)

/// Get the bigger value between a and b.
#define max(a, b) \
	({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a > _b ? _a : _b; })

/// Get the smaller value between a and b.
#define min(a, b) \
	({ __typeof__ (a) _a = (a); \
	__typeof__ (b) _b = (b); \
	_a < _b ? _a : _b; })

/**
 * The global context structure.
 *
 * Keeps track of all variables which construct the current state of the program.
 * It is good to have no global variables, this structure minimizes the count to two.
 * */
struct AS_GlobalCtx {
	// Editor Screen
	char editor_scr_message[1024];                             /// Current message to be displated at the bottom of the editor screen
	
	// Interface
	struct AS_RenderCtx render_ctx;                            /// The current render context.
	struct AS_Screen screens[AS_MAX_SCREEN_COUNT];             /// A list of all registered screens.
	struct AS_Screen *screen;                                  /// A pointer to the currently active screen.

	// Editor
	struct AS_TextFile *text_file_head;                        /// The start of the linked list containing all text files.
	struct AS_TextFile *text_file;                             /// The current text file.

	// Columns
	struct AS_ColDesc col_descs[AS_MAX_COLUMNS];               /// An array of all available column descriptors / layouts.
	int col_desc_i;                                            /// The index of the currently selected column descriptor.

	// Syntax
	struct AS_SyntaxBackendMeta syn_backends[AS_MAX_BACKENDS]; /// All registered syntax backends.
};

/**
 * The global context variable.
 * */
extern struct AS_GlobalCtx as_ctx;

#endif
