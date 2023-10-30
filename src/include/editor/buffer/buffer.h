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

#define MAX_BUFFERS 32

#include <global.h>

struct line_list_element {
        char *contents;
        int line;
        struct line_list_element *next;
};

struct text_buffer {
        int cx;
        int cy;
        struct line_list_element *head;
};

struct text_file {
        char *name;
        FILE *file;
        int load_offset;
        struct text_buffer *buffers;
        struct text_buffer *active_buffer;
};

struct syntax_highlight {
        char *extension;
        char **symbols;
};

struct text_buffer *new_buffer();
void destroy_buffer(struct text_buffer *buffer);

void buffer_char_insert(char c);
void buffer_char_del();

#endif