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

/// Maximum number of column descriptors in as_ctx.col_descs.
#define AS_MAX_COLUMNS 32

#include <editor/config.h>
#include <editor/buffer/buffer.h>

#include <includes.h>

/**
 * Describes a single open file.
 * */
struct AS_TextFile {
	/// 0-based index of current line.
        int cy;
	/// Number of buffers selected.
	int selected_buffers;
	/// 0-based index of the active buffer.
	int active_buffer_idx;
	/// Number of buffers present in buffers list (as_ctx.col_descs[as_ctx.col_desc_i].column_count).
	int buffer_count;
	/// The offset within
        int load_offset;
	/// Path to the file.
        char *name;
	/// Pointer to the open file.
        FILE *file;
	/// Array of pointers to all buffers.
        struct AS_TextBuf **buffers;
	/// The buffer which is currently selected by the user.
        struct AS_TextBuf *active_buffer;
	/// Pointer to the next struct AS_TextFile, NULL if this is the last open file.
	struct AS_TextFile *next;
	/// Pointer to the previous struct AS_TextFile, NULL if this is the first open file.
	struct AS_TextFile *prev;
};

/**
 * Describes in the layout of all columns.
 * */
struct AS_ColDesc {
	/// Array of indices describing the start of each column.
        int *column_positions;
	/// The number of columns in column_positions array.
        int column_count;
	/// The character by which each column is separated.
        int delimiter;
};

/**
 * Load a file into a `struct AS_TextFile`.
 *
 * Loads the file from the given file path into the internal `struct AS_TextBuf` and
 * as `struct AS_TextFile` representations.
 *
 * @param char *name - Path to the file.
 * @return A pointer to the internal structure describing the file.
 * */
struct AS_TextFile *load_file(char *name);

/**
 * Reloads the given file.
 *
 * @param struct AS_TextFile *file - The file to reload. */
void reload_file(struct AS_TextFile *file);

/**
 * Wrapper of `reload_file` to reload all files in the list as_ctx.text_file_head.
 * */
void reload_all();

/**
 * Save the given file.
 *
 * @param struct AS_TextFile * - The file to save.
 * */
void save_file(struct AS_TextFile *file);

/**
 * Wrapper of save_file to save all files in the list as_ctx.text_file_head.
 * */
void save_all();

/**
 * Destroy the given file
 *
 * Free the memory of the given file.\n
 * This does not save the contents of the file in memory to the file on disk.
 *
 * @param struct AS_TextFile *file - The file to destroy.
 * */
void destroy_file(struct AS_TextFile *file);

/**
 * Wrapper of destroy_file to destroy all files in the list as_ctx.text_file_head.
 * */
void destroy_all();

/**
 * Function to configure the editor.
 *
 * This function is invoked by config.c's interpret_token_stream
 * function. The current element is passed, which should be of type
 * AS_CFG_TOKEN_KEY and of value AS_CFG_LOOKUP_COLUMNS.
 *
 * This function will initialize the user's columns into as_ctx.col_descs.
 *
 * @param struct AS_CfgTok *token - Pointer to starting token of the editor's configuration.
 * @return A pointer to the last element of the set, caller must invoke NEXT_TOKEN to be placed
 * at the beginning of the next set.
 * */
struct AS_CfgTok *configure_editor(struct AS_CfgTok *token);

#endif
