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

#ifndef BUFFER_H
#define BUFFER_H

#include <global.h>
#include <interface/interface.h>

struct line_list_element {
        char *contents;
        struct line_list_element *next;
        struct line_list_element *prev;
};

struct text_buffer {
        int cx;
        int col_start;
        int col_end;

	uint8_t selection_enabled;
	struct bound selection_start;
	struct bound selection_end;

        struct line_list_element *head;
        struct line_list_element *current_element;
	struct line_list_element *selection_start_line;
};

struct syntax_highlight {
        char *extension;
        char **symbols;
};

struct text_buffer *new_buffer(int col_start, int col_end);
void destroy_buffer(struct text_buffer *buffer);

void buffer_char_insert(char c);
void buffer_char_del();
int buffer_move_ln_down(struct text_buffer *active_buffer);
int buffer_move_ln_up(struct text_buffer *active_buffer);

#endif
