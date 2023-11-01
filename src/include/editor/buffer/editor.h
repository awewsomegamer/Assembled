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

#ifndef EDITOR_H
#define EDITOR_H

#define MAX_TEXT_FILES 32
#define MAX_COLUMNS 32

#include <editor/config.h>
#include <editor/buffer/buffer.h>

struct text_file {
        char *name;
        FILE *file;
        int load_offset;
        int cy;
        struct text_buffer **buffers;
        struct text_buffer *active_buffer;
};

struct column_descriptor {
        int column_count;
        int *column_positions;
        int delimiter;     
};
extern struct column_descriptor column_descriptors[];

extern struct text_file *active_text_file;

struct text_file *load_file(char *name);
void save_file(struct text_file *file);
void edit_file();

struct cfg_token *init_editor(struct cfg_token *token);

#endif