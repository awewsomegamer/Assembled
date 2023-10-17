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

struct key_layer {
        void (*function[128])(); // The index within the array is the ASCII code
        struct key_layer *next;
        DEBUG_CODE( int level; )
};
static struct key_layer top_layer;

static int key_layer_count = 0;

struct key_stack_element {
        char key;
        time_t time;
};
static struct key_stack_element key_stack[MAX_KEY_ELEMENTS];
static int key_stack_ptr = 0;

// Collapse (interpet) current key stack, looking
// for any valid combinations of keys.
// TODO: Ensure the stack won't stick together
//       and lock the user from enter more keys.
//       Possible fix: make the stack bigger.
void collapse_stack() {
        // Start at top layer
        struct key_layer *layer = &top_layer;

        // Increment through list of pressed keys
        for (int i = 0; i < key_stack_ptr; i++) {
                // Retrieve current function of the key on the current layer
                void (*func_ptr)() = layer->function[key_stack[i].key];
                
                // NULL? Empty stack and return
                if (func_ptr == NULL) {
                        key_stack_ptr = 0;
                        return;
                }

                // Layer down? Change current layer to next layer and continue to next char
                if (func_ptr == layer_down) {
                        layer = layer->next;
                        continue;
                }

                // Finally found the function, call it and reset stack
                (func_ptr)();
                key_stack_ptr = 0;
        }
}

// Acknowledge a key, put it on the stack, ask
// to collapse the stack.
void key(char c) {
        // Wrap around (should never happen unless user has more
        //              keys than MAX_KEY_ELEMENTS)
        if (key_stack_ptr >= MAX_KEY_ELEMENTS) {
                key_stack_ptr = 0;
        }

        // Push new value to stack
        key_stack[key_stack_ptr].key = c;
        key_stack[key_stack_ptr++].time = time(NULL);
        
        // Try to collapse the stack
        collapse_stack();
}

// Initialize the keyboard handler, import
// key combbinations from the configuration
// file into a data structure.
struct cfg_token *configure_keyboard(struct cfg_token *token) {
        EXPECT_TOKEN(CFG_TOKEN_KEY, "Expected keyword")
        EXPECT_VALUE(CFG_LOOKUP_KEYSEQ, "Expected keyword keyseq")

        NEXT_TOKEN
        EXPECT_TOKEN(CFG_TOKEN_STR, "Expected string")

        DEBUG_MSG("Function name: \"%s\", Hash: %lu, Index: %d\n", token->str, general_hash(token->str), GET_FUNC_IDX(token->str));
        void (*function)() = functions[GET_FUNC_IDX(token->str)];

        NEXT_TOKEN
        EXPECT_TOKEN(CFG_TOKEN_COL, "Expected colon")

        NEXT_TOKEN

        DEBUG_MSG("Stack trace for function %X (LD: %X):\n", function, layer_down);

        struct key_layer *current_layer = &top_layer;
        while (token->type == CFG_TOKEN_INT || token->type == CFG_TOKEN_COM) {
                if (token->type == CFG_TOKEN_COM) {
                        NEXT_TOKEN
                        continue;
                }

                if (current_layer->next == NULL) {
                        current_layer->next = (struct key_layer *)malloc(sizeof(struct key_layer));
                        memset(current_layer->next, 0, sizeof(struct key_layer));
                }

                current_layer->function[token->value] = (token->next->type != CFG_TOKEN_INT || token->next->type != CFG_TOKEN_COM)
                                                        ? function : layer_down; 

                current_layer = current_layer->next;
                
                DEBUG_MSG("%X\n", token->value);

                NEXT_TOKEN
        }

        DEBUG_MSG("Stack end\n")
        

        return token;
}