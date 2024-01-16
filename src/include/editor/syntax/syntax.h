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

#ifndef AS_SYNTAX_H
#define AS_SYNTAX_H

#define AS_MAX_BACKENDS 256

#include <editor/buffer/editor.h>
#include <editor/buffer/buffer.h>

enum {
	AS_SYNTAX_TYPE_ASM,
};

static const char *As_SyntaxExtNames[] = {
	[AS_SYNTAX_TYPE_ASM] = "asm",
};

struct AS_SyntaxBackendMeta {
	struct AS_SyntaxPoint *(*get_syntax)(char *);
	int extensions[32];
};

void init_syntax();
struct AS_SyntaxPoint *get_syntax(struct AS_TextFile *file, struct AS_LLElement *element);

#endif
