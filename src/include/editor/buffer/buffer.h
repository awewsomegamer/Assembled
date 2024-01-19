/**
 * @file buffer.h
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
 * This file handles actions related to `struct AS_TextBuf`s, such as character insertion
 * and deletion.
 *
*/

#ifndef AS_BUFFER_H
#define AS_BUFFER_H

#include <interface/interface.h>

#include <includes.h>

/**
 * Syntax points for describing highlighting.
 *
 * A linked list which describes regions of text and
 * their color pairs.
 * */
struct AS_SyntaxPoint {
	int x;                                             /// Offset into AS_TextBuf->contents at which this applies
	int length;                                        /// The number of characters that are included within the region (minimum: 1)
	int color;                                         /// The index of the color pair for this region
	struct AS_SyntaxPoint *next;                       /// A pointer to the next syntax point, NULL if this is the last point
};

/**
 * A single line element.
 *
 * A linked list which holds the all of the lines of
 * a column. */
struct AS_LLElement {
        char *contents;                                     /// Contains a single line of the open file (zero terminated, with no '\n' character).
        struct AS_LLElement *next;                          /// A pointer to the next line, NULL if this is the last line.
        struct AS_LLElement *prev;                          /// A pointer to the previous line, NULL if this is the first line.
	struct AS_SyntaxPoint *syntax;                      /// A pointer to a linked list outlining how certain regions of `contents` is supposed to be colored.
};

/**
 * A text buffer, or column.
 *
 * Holds all information about a single column.
 *  */
struct AS_TextBuf {
        int cx;                                             /// Cursor's X position in buffer.
        int col_start;                                      /// The start index of the buffer's column.
        int col_end;                                        /// The end index of the buffer's column.

	uint8_t selection_enabled;                          /// Determines if selection is enabled for this buffer (1).
	struct AS_Bound selection_start;                    /// 0-based coordinates of the selection's start.

        struct AS_LLElement *head;                          /// The first line in the buffer.
	struct AS_LLElement *virtual_head;                  /// The first line on screen.
        struct AS_LLElement *current_element;               /// Pointer to the line at (cx, cy).
	struct AS_LLElement *selection_start_line;          /// Pointer to the line at (0, selection_start.y).
};

/**
 * Creates a new `struct AS_TextBuf`
 *
 * Creates a new, and initializes all fields of a new `struct AS_TextBuf`.
 *
 * @param int col_start - The 0-based index where the buffer's column starts
 * @param int col_end - The 0-based index where the buffer's column ends
 * @return The pointer to the struct AS_TextBuf
 *  */
struct AS_TextBuf *new_buffer(int col_start, int col_end);
/**
 * Frees a `struct AS_TextBuf`
 *
 * @param struct AS_TextBUf *buffer - The buffer to be freed.
 * */
void destroy_buffer(struct AS_TextBuf *buffer);

/**
 * Inserts a new character into the active buffer.
 *
 * Insert the given character into as_ctx.text_file->active_buffer->current_element.
 *
 * @param char c - The character to be inserted. If it is '\\n', then a new struct AS_LLElement
 * will be inserted.
 * */
void buffer_char_insert(char c);

/**
 * Deletes the current character.
 *
 * Deletes the current character in as_ctx.text_file->active_buffer->current_element.
 * If `current_element` is completely empty, then it will be deleted.
 * */
void buffer_char_del();

/**
 * Moves the current line in the given buffer down.
 *
 * @param struct AS_TextBuf *active_buffer - The buffer in which to move active_buffer->current_element
 * down one.
 * @return Returns the number of lines moved down (0: if the function failed).
 * */
int buffer_move_ln_down(struct AS_TextBuf *active_buffer);

/**
 * Moves the current line in the given buffer up.
 *
 * @param struct AS_TextBuf *active_buffer - The buffer in which to move active_buffer->current_element
 * up one.
 * @return Returns the number of lines moved up (0: if the function failed).
 * */
int buffer_move_ln_up(struct AS_TextBuf *active_buffer);

#endif
