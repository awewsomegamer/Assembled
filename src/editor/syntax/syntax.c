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

#include <editor/buffer/buffer.h>
#include <editor/syntax/syntax.h>
#include <editor/syntax/backends/nasm.h>

#include <string.h>
#include <includes.h>
#include <global.h>

static int backend_count = 0;

void init_syntax() {
	as_asm_backend_init(backend_count++);
}

struct AS_SyntaxPoint *get_syntax(struct AS_TextFile *file, struct AS_LLElement *element) {
	// Find the index of the first '.' from the end of the file name
	int dot;
	for (dot = strlen(file->name) - 1; dot >= 0; dot--) {
		if (file->name[dot] == '.') {
			dot++;
			break;
		}
	}

	// Copy out the extension
	char *extension = (char *)malloc(strlen(file->name) - dot);
	strcpy(extension, file->name + dot);

	// TODO: This can be further optimized, structures need to be changed.

	// Look at each backend's supported extensions list
	for (int i = 0; i < backend_count; i++) {
		for (int j = 0; j < AS_MAX_BACKENDS; j++) {
			const char *string = As_SyntaxExtNames[as_ctx.syn_backends[i].extensions[j]];

			if (*string == *extension && strcmp(string, extension) == 0) {
				// Extensions match, get syntax
				return as_ctx.syn_backends[i].get_syntax(element->contents);
			}
		}
	}

	// Extension didn't match any backend, no syntax
	return NULL;
}
