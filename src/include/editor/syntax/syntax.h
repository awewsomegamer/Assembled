/**
 * @file syntax.h
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
 * Responsible for selecting the proper syntax backend for a given file, and installing
 * all syntax backends into as_ctx.syn_backends.
*/

#ifndef AS_SYNTAX_H
#define AS_SYNTAX_H

/// Maximum number of syntax backends.
#define AS_MAX_BACKENDS 256
/// Maximum number of extensions per backend.
#define AS_MAX_BACKEND_EXTS 32

#include <editor/buffer/editor.h>
#include <editor/buffer/buffer.h>

/**
 * Indices used for extension names.
 * */
enum AS_SYNTAX_TYPE {
	AS_SYNTAX_TYPE_ASM,
};

/**
 * Names of extensions
 * */
static const char *As_SyntaxExtNames[] = {
	[AS_SYNTAX_TYPE_ASM] = "asm",
};

/**
 * Representation of a backend
 * */
struct AS_SyntaxBackendMeta {
	/// The function to call to get syntax information.
	struct AS_SyntaxPoint *(*get_syntax)(char *);
	/// The number of file extensions this backend handles.
	int extensions[AS_MAX_BACKEND_EXTS];
};

/**
 * Initalize as_ctx.syn_backends.
 * */
void init_syntax();

/**
 * Wrapper function to get syntax for a file.
 *
 * This function determines which syntax backend to use based on the
 * file's extension.
 *
 * @param struct AS_TextFile *file - File in which next parameter is in.
 * @param struct AS_LLElement *element - The line for which to create a syntax point linked list.
 * */
struct AS_SyntaxPoint *get_syntax(struct AS_TextFile *file, struct AS_LLElement *element);

#endif
