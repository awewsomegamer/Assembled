/**
 * @file editor.h
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
 * Handles files.
*/

#ifndef AS_EDITOR_H
#define AS_EDITOR_H

#define AS_MAX_COLUMNS 32                             /// Maximum number of column descriptors in as_ctx.col_descs.

#include <editor/config.h>
#include <editor/buffer/buffer.h>

#include <includes.h>

/**
 * Describes a single open file.
 * */
struct AS_TextFile {
        int cy;                                        /// 0-based index of current line.
	int selected_buffers;                          /// Number of buffers selected.
	int active_buffer_idx;                         /// 0-based index of the active buffer.
	int buffer_count;                              /// Number of buffers present in buffers list (as_ctx.col_descs[as_ctx.col_desc_i].column_count).
        int load_offset;                               /// The offset within
        char *name;                                    /// Path to the file.
        FILE *file;                                    /// Pointer to the open file.
        struct AS_TextBuf **buffers;                   /// Array of pointers to all buffers.
        struct AS_TextBuf *active_buffer;              /// The buffer which is currently selected by the user.
	struct AS_TextFile *next;                      /// Pointer to the next struct AS_TextFile, NULL if this is the last open file.
	struct AS_TextFile *prev;                      /// Pointer to the previous struct AS_TextFile, NULL if this is the first open file.
};

/**
 * Describes in the layout of all columns.
 * */
struct AS_ColDesc {
        int *column_positions;                         /// Array of indices describing the start of each column.
        int column_count;                              /// The number of columns in column_positions array.
        int delimiter;                                 /// The character by which each column is separated.
};

/**
 * Load a file into a `struct AS_TextFile`.
 *
 * */
struct AS_TextFile *load_file(char *name);
void reload_file(struct AS_TextFile *file);
void reload_all();
void save_file(struct AS_TextFile *file);
void save_all();
void destroy_file(struct AS_TextFile *file);
void destroy_all();

struct AS_CfgTok *configure_editor(struct AS_CfgTok *token);

#endif
