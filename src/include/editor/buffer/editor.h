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

#ifndef AS_EDITOR_H
#define AS_EDITOR_H

#define AS_MAX_COLUMNS 32

#include <editor/config.h>
#include <editor/buffer/buffer.h>

#include <includes.h>

struct AS_TextFile {
        int cy;
	int selected_buffers;
	int active_buffer_idx;
	int buffer_count;
        int load_offset;
        char *name;
        FILE *file;
        struct AS_TextBuf **buffers;
        struct AS_TextBuf *active_buffer;
	struct AS_TextFile *next;
	struct AS_TextFile *prev;
};

struct AS_ColDesc {
        int *column_positions;
        int column_count;
        int delimiter;     
};

struct AS_TextFile *load_file(char *name);
void reload_file(struct AS_TextFile *file);
void reload_all();
void save_file(struct AS_TextFile *file);
void save_all();
void destroy_file(struct AS_TextFile *file);
void destroy_all();

struct AS_CfgTok *configure_editor(struct AS_CfgTok *token);

#endif
