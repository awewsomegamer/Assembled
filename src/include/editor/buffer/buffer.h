/*
*    Assembled - Column based text editor
*    Copyright (C) 2023-2024 awewsomegamer
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

#ifndef AS_BUFFER_H
#define AS_BUFFER_H

#include <interface/interface.h>

#include <includes.h>

struct AS_SyntaxPoint {
	int x;
	int length;
	int color;
	struct AS_SyntaxPoint *next;
};

struct AS_LLElement {
        char *contents;
        struct AS_LLElement *next;
        struct AS_LLElement *prev;
	struct AS_SyntaxPoint *syntax;
};

struct AS_TextBuf {
        int cx;                                             // Cursor's X position in buffer
        int col_start;                                      // The start index of the buffer's column
        int col_end;                                        // The end index of the buffer's column

	uint8_t selection_enabled;                          // Selection is enabled for this buffer
	struct AS_Bound selection_start;                    // X and Y coordinates of the selection's start

        struct AS_LLElement *head;                          // The first line in the buffer
	struct AS_LLElement *virtual_head;                  // The first line on screen
        struct AS_LLElement *current_element;               // Pointer to the line at (cx, cy)
	struct AS_LLElement *selection_start_line;          // Pointer to the line at (0, selection_start.y)
};

struct AS_TextBuf *new_buffer(int col_start, int col_end);
void destroy_buffer(struct AS_TextBuf *buffer);

void buffer_char_insert(char c);
void buffer_char_del();
int buffer_move_ln_down(struct AS_TextBuf *active_buffer);
int buffer_move_ln_up(struct AS_TextBuf *active_buffer);

#endif
