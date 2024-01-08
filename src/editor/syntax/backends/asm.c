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

#include "editor/syntax/syntax.h"
#include "includes.h"
#include "interface/theming/themes.h"
#include <editor/buffer/buffer.h>
#include <editor/syntax/backends/asm.h>
#include <global.h>

struct keyword {
	const char *word;
	int color;
};

enum {
	INSTRUCTION = AS_CUSTOM_COLOR_START,
	LABEL = INSTRUCTION + 1,
	COLON = LABEL + 1,
	SQUARE = COLON + 1,
};

struct keyword keywords[] = {
//	{ "mov", INSTRUCTION },
};

// ERROR: Now that the update function uses this function
//        and possibly even the rendering code relying on
//        the list produced by the following code, the user
//        cannot type spaces, typing mov with the above table's
//        element uncommented all result in the same error:
//        Fatal glibc error: malloc.c:2594 (sysmalloc):
//        assertion failed: (old_top == initial_top (av)
//        && old_size == 0) || ((unsigned long) (old_size)
//        >= MINSIZE && prev_inuse (old_top) &&
//        ((unsigned long) old_end & (pagesize - 1)) == 0)
struct AS_SyntaxPoints *as_asm_get_syntax(char *line) {
	char c = 0;
	int x = 0;

	struct AS_SyntaxPoints *head = (struct AS_SyntaxPoints *)malloc(sizeof(struct AS_SyntaxPoints));
	memset(head, 0, sizeof(struct AS_SyntaxPoints));
	struct AS_SyntaxPoints *current = head;

	// ERROR: This loop gets stuck sometimes
	while ((c = *line)) {
		current->length = 1;

		switch (c) {
		case ':': {
			current->color = COLON;

			break;
		}

		case '[':
		case ']': {
			current->color = SQUARE;

			break;
		}

		default: {
			break;
			int i = 0;
			char n = *(line + i);
			char *start = line;

			while (isalnum(n) || n == '_') {
				i++;
				n = *(line + i);
			}

			char *extracted = (char *)malloc(i);
			strncpy(extracted, start, i);
			extracted[i] = 0;
			current->length = i - 1;

			for (int j = 0; j < (sizeof(keywords)/sizeof(keywords[0])); j++) {
				if (strcmp(extracted, keywords[j].word) == 0) {
					current->color = LABEL;
					break;
				}
			}

			line += i - 1;
			free(extracted);

			break;
		}
		}

		current->x = x++;
		current->next = (struct AS_SyntaxPoints *)malloc(sizeof(struct AS_SyntaxPoints));
		memset(current->next, 0, sizeof(struct AS_SyntaxPoints));
		current = current->next;

		line++;
	}

	return head;
}

void as_asm_backend_init(int i) {
	as_ctx.syn_backends[i].extensions[0] = AS_SYNTAX_TYPE_ASM;
	as_ctx.syn_backends[i].get_syntax = as_asm_get_syntax;
}
