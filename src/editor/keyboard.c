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

#include <interface/interface.h>
#include <editor/buffer/buffer.h>
#include <ctype.h>
#include <curses.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <global.h>
#include <editor/keyboard.h>
#include <editor/functions.h>
#include <editor/config.h>

struct keyseq_list {
	int code;
	struct keyseq_list *next;
};

struct keyseq {
	struct keyseq_list *list;
	void (*function)();
	struct keyseq *next;
};

static struct keyseq keyseq_list_head = { 0 };
static struct keyseq *keyseq_list_last = &keyseq_list_head;

struct key_stack_element {
        int key;
        time_t time;
};

static struct key_stack_element key_stack[MAX_KEY_ELEMENTS];
static int key_stack_ptr = 0;

// Collapse (interpet) current key stack, looking
// for any valid combinations of keys.
void collapse_stack() {
	struct keyseq *current = &keyseq_list_head;

	while (current != NULL) {
		struct keyseq_list *element = current->list;
		int i = 0;
		
		for (; i < key_stack_ptr; i++) {
			if (element == NULL || key_stack[i].key != element->code) {
				break;
			}

			element = element->next;
		}

		if (i == key_stack_ptr && element == NULL) {
			// Found function
			(*current->function)();

			key_stack_ptr = 0;

			return;
		} else if (i > 0) {
			return;
		}

		current = current->next;
	}
	
	as_ctx.screen->local(LOCAL_BUFFER_CHAR, key_stack[--key_stack_ptr].key);
	key_stack_ptr = 0;
}

// Acknowledge a key, put it on the stack, ask
// to collapse the stack.
// CONVIENENCE: Create an End of Command char
//              that will tell us when to
//              collapse the stack
void key(int c) {
        // Wrap around (should never happen unless user has more
        //              keys than MAX_KEY_ELEMENTS)
        if (key_stack_ptr >= MAX_KEY_ELEMENTS) {
                key_stack_ptr = 0;
        }

	AS_DEBUG_MSG("Key pressed: %d\n", c);

        // Push new value to stack
        key_stack[key_stack_ptr].key = c;
        key_stack[key_stack_ptr++].time = time(NULL);
        
        // Try to collapse the stack
        collapse_stack();
}

// Initialize the keyboard handler, import
// key combbinations from the configuration
// file into a data structure.
struct AS_CfgTok *configure_keyboard(struct AS_CfgTok *token) {
        AS_EXPECT_TOKEN(AS_CFG_TOKEN_KEY, "Expected keyword")
        AS_EXPECT_VALUE(AS_CFG_LOOKUP_KEYSEQ, "Expected keyword keyseq")

        AS_NEXT_TOKEN
        AS_EXPECT_TOKEN(AS_CFG_TOKEN_KEY, "Expected keyword")

        void (*function)() = as_ctx.functions[token->value];

        AS_NEXT_TOKEN
        AS_EXPECT_TOKEN(AS_CFG_TOKEN_COL, "Expected colon")

        AS_NEXT_TOKEN

        AS_DEBUG_MSG("Stack trace for function %X (LD: %X):\n", function, layer_down);

	struct keyseq_list *list = (struct keyseq_list *)malloc(sizeof(struct keyseq_list));
	struct keyseq_list *current = list;
	
	while (token->type == AS_CFG_TOKEN_INT) {
		if (token->type == AS_CFG_TOKEN_COM) {
			AS_NEXT_TOKEN
			continue;
		}

		current->code = token->value;
		current->next = NULL;
		
		AS_NEXT_TOKEN

		while (token->type == AS_CFG_TOKEN_COM) {
			AS_NEXT_TOKEN
		}

		if (token->type != AS_CFG_TOKEN_INT) {
			break;
		}

		current->next = (struct keyseq_list *)malloc(sizeof(struct keyseq_list));
		current = current->next;
	}

	keyseq_list_last->list = list;
	keyseq_list_last->function = function;
	keyseq_list_last->next = (struct keyseq *)malloc(sizeof(struct keyseq));
	keyseq_list_last = keyseq_list_last->next;

        AS_DEBUG_MSG("Stack end\n")
        

        return token;
}