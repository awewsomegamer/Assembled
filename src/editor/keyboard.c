/**
 * @file keyboard.c
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
 * Responsible for handling all keyboard input and determining when the user
 * has entered a key sequence for the active screen to preform an action.
*/

#include <includes.h>
#include <editor/buffer/buffer.h>
#include <editor/keyboard.h>
#include <editor/config.h>

#include <interface/interface.h>

#include <global.h>
#include <stdio.h>

/// Maximum number of functions.
#define MAX_FUNCTION_COUNT 256
#define PARAM2(a, b) { a, b },

/**
 * A list which represents the order of keys the user needs to press.
 * */
struct AS_KeySeqList {
	/// The keycode the user needs to press.
	int code;
	/// Pointer to the next element.
	struct AS_KeySeqList *next;
};

/**
 * Internal structure defining actions to a AS_KeySeqList structure.
 *
 * Defines the set of keys which need to be pressed by the user to activate a
 * given funcntion on the active screen.
 * */
struct AS_KeySeq {
	/// The sequence of keys which need to be pressed by the user.
	struct AS_KeySeqList *list;
	/// The local function which should be called.
	int function;
	/// Pointer to the next element.
	struct AS_KeySeq *next;
};

/**
 *  */
static struct AS_KeySeq keyseq_list_head = { 0 };
/**
 * */
static struct AS_KeySeq *keyseq_list_last = &keyseq_list_head;

/**
 * Element on a LIFO stack of the users inputs. */
struct AS_KeyStackElem {
	/// The key the user pressed.
        int key;
	/// The time the usre pressed the key.
        time_t time;
};

/**
 * LIFO stack of user's key presses.
 * */
static struct AS_KeyStackElem key_stack[AS_MAX_KEY_ELEMENTS];
/**
 * Stack pointer of key_stack
 * */
static int key_stack_ptr = 0;

/**
 * Arguments for local functions depending on action type.
 *
 * Outlines the two arguments which need to be submitted to the active screen's
 * local function for the given action. The list is indexed with AS_KeySeq->function.
 * */
static const int func_args[MAX_FUNCTION_COUNT][2] = {
	[AS_CFG_LOOKUP_UP]            = PARAM2(LOCAL_ARROW_YMOVE, -1)
        [AS_CFG_LOOKUP_DOWN]          = PARAM2(LOCAL_ARROW_YMOVE, 1)
        [AS_CFG_LOOKUP_LEFT]          = PARAM2(LOCAL_ARROW_XMOVE, -1)
	[AS_CFG_LOOKUP_RIGHT]         = PARAM2(LOCAL_ARROW_XMOVE, 1)
        [AS_CFG_LOOKUP_ENTER]         = PARAM2(LOCAL_ENTER, 0)
	[AS_CFG_LOOKUP_BUFFER_LEFT]   = PARAM2(LOCAL_BUFFER_MOVE, -1)
	[AS_CFG_LOOKUP_BUFFER_RIGHT]  = PARAM2(LOCAL_BUFFER_MOVE, 1)
	[AS_CFG_LOOKUP_WINDOW_LEFT]   = PARAM2(LOCAL_WINDOW_MOVE, -1)
	[AS_CFG_LOOKUP_WINDOW_RIGHT]  = PARAM2(LOCAL_WINDOW_MOVE, 1)
	[AS_CFG_LOOKUP_FILE_SAVE]     = PARAM2(LOCAL_FILE_SAVE, 0)
	[AS_CFG_LOOKUP_FILE_SAVE_ALL] = PARAM2(LOCAL_FILE_SAVE, 1)
	[AS_CFG_LOOKUP_FILE_LOAD]     = PARAM2(LOCAL_FILE_LOAD, 0)
	[AS_CFG_LOOKUP_SELECTION]     = PARAM2(LOCAL_WINDOW_SELECTION, 1)
	[AS_CFG_LOOKUP_MOVE_LN_UP]    = PARAM2(LOCAL_BUFFER_MOVE_LINE, 1)
	[AS_CFG_LOOKUP_MOVE_LN_DOWN]  = PARAM2(LOCAL_BUFFER_MOVE_LINE, 0)
	[AS_CFG_LOOKUP_COLDESC_LEFT]  = PARAM2(LOCAL_COLDESC_SWITCH, -1)
	[AS_CFG_LOOKUP_COLDESC_RIGHT] = PARAM2(LOCAL_COLDESC_SWITCH, 1)
};

/**
 * Collapse or interpret the current key stack.
 *
 * This function looks from the current key_stack_ptr to 0 seeing if it can
 * detect an AS_KeySeq within it. If it can, the stack pointer is set to 0,
 * and the active screen's local function is called. In the event that it
 * detects the beginnings of a key sequence (that is not yet complete), it
 * will return, leaving the stack pointer intact. Otherwise, it will default
 * to calling LOCAL_BUFFER_CHAR with the character at the stack pointer. */
void collapse_stack() {
	struct AS_KeySeq *current = &keyseq_list_head;

	while (current != NULL) {
		struct AS_KeySeqList *element = current->list;
		int i = 0;
		
		for (; i < key_stack_ptr; i++) {
			if (element == NULL || key_stack[i].key != element->code) {
				break;
			}

			element = element->next;
		}

		if (i == key_stack_ptr && element == NULL) {
			// Found function
			AS_DEBUG_MSG("Calling function %d\n", current->function);
			as_ctx.screen->local(func_args[current->function][0], func_args[current->function][1]);

			key_stack_ptr = 0;

			return;
		} else if (i > 0) {
			return;
		}

		current = current->next;
	}

	AS_DEBUG_MSG("Putting key %d\n", key_stack[key_stack_ptr - 1].key);
	as_ctx.screen->local(LOCAL_BUFFER_CHAR, key_stack[--key_stack_ptr].key);
	key_stack_ptr = 0;
}

// CONVIENENCE: Create an End of Command char
//              that will tell us when to
//              collapse the stack
void key(int c) {
        // Wrap around (should never happen unless user has more
        //              keys than MAX_KEY_ELEMENTS)
        if (key_stack_ptr >= AS_MAX_KEY_ELEMENTS) {
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

	int function = token->value;

        AS_NEXT_TOKEN
        AS_EXPECT_TOKEN(AS_CFG_TOKEN_COL, "Expected colon")

        AS_NEXT_TOKEN

	struct AS_KeySeqList *list = (struct AS_KeySeqList *)malloc(sizeof(struct AS_KeySeqList));
	struct AS_KeySeqList *current = list;
	
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

		current->next = (struct AS_KeySeqList *)malloc(sizeof(struct AS_KeySeqList));
		current = current->next;
	}

	keyseq_list_last->list = list;
	keyseq_list_last->function = function;
	keyseq_list_last->next = (struct AS_KeySeq *)malloc(sizeof(struct AS_KeySeq));
	keyseq_list_last = keyseq_list_last->next;

        AS_DEBUG_MSG("Stack end\n")

        return token;
}
