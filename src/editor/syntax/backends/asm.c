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
#include <editor/buffer/buffer.h>
#include <editor/syntax/backends/asm.h>
#include <global.h>

struct keyword {
	const char *word;
	int color;
};

enum {
	INSTRUCTION = 16,
	LABEL = 17,
	COLON = 18,
};

struct keyword keywords[] = {
	{ "mov", INSTRUCTION },
};

struct AS_SyntaxPoints *as_asm_get_syntax(char *line) {
	char c = 0;
	int x = 0;

	struct AS_SyntaxPoints *head = (struct AS_SyntaxPoints *)malloc(sizeof(struct AS_SyntaxPoints));
	head->next = head;

	struct AS_SyntaxPoints *current = head;

	while ((c = *line)) {
		int color = 0;
		int length = 1;

		switch (c) {
		case ':': {
			current->color = LABEL;
			color = COLON;

			break;
		}

		default: {
			int i = 0;
			char n = *(line + i);
			char *start = line;

			while (isalnum(n) || n == '_') {
				i++;
				n = *line++;
			}

			i--;
			line -= 2;

			char *extracted = (char *)malloc(i + 1);
			strncpy(extracted, start, i);
			extracted[i] = 0;
			length = i;

			for (int j = 0; j < (sizeof(keywords)/sizeof(keywords[0])); j++) {
				if (strcmp(extracted, keywords[j].word) == 0) {
					color = keywords[j].color;
					break;
				}
			}

			break;
		}
		}

		current = current->next;
		current->color = color;
		current->x = x++;
		current->length = length;
		current->next = (struct AS_SyntaxPoints *)malloc(sizeof(struct AS_SyntaxPoints));

		line++;
	}

	return head;
}

void as_asm_backend_init(int i) {
	as_ctx.syn_backends[i].extensions[0] = AS_SYNTAX_TYPE_ASM;
	as_ctx.syn_backends[i].get_syntax = as_asm_get_syntax;
}
