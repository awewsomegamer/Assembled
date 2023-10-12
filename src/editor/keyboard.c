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

struct key_layer {
        void (*function[128])(); // The index within the array is the ASCII code
        struct key_layer *next;
        DEBUG_CODE( int level; )
};
struct key_layer top_layer;

struct key_stack_element {
        char key;
        time_t time;
};
struct key_stack_element key_stack[MAX_KEY_ELEMENTS];
int key_stack_ptr = 0;

// Collapse (interpet) current key stack, looking
// for any valid combinations of keys.
// TODO: Ensure the stack won't stick together
//       and lock the user from enter more keys.
//       Possible fix: make the stack bigger.
void collapse_stack() {
        struct key_layer *layer = &top_layer;

        for (int i = 0; i < key_stack_ptr; i++) {
                void (*func_ptr)() = layer->function[key_stack[i].key];

                if (func_ptr == NULL) {
                        key_stack_ptr = 0;
                        return;
                }

                if (func_ptr == layer_down) {
                        layer = layer->next;
                        continue;
                }

                (func_ptr)();
                key_stack_ptr = 0;
        }
}

// Acknowledge a key, put it on the stack, ask
// to collapse the stack.
void key(char c) {
        if (key_stack_ptr >= MAX_KEY_ELEMENTS) {
                key_stack_ptr = 0;
        }

        key_stack[key_stack_ptr].key = c;
        key_stack[key_stack_ptr++].time = time(NULL);
        collapse_stack();
}

void create_path(char *start, char *end, void (*function)(), struct key_layer *layer) {    
        char *i = start;

        for (; i < end; i++) {
                if (*i != ',')
                        continue;

                if (layer->next == NULL) {
                        layer->next = (struct key_layer *)malloc(sizeof(struct key_layer));
                        DEBUG_CODE( layer->next->level = layer->level + 1; )
                        memset(layer->next, 0, sizeof(struct key_layer));
                }

                create_path(i + 1, end, function, layer->next);
                function = layer_down;
        }

        for (; (isblank(*start)) && (start < end); start++);

        DEBUG_CODE( int func_idx = 0; )

        switch (*start) {
        case '\'': {
                // Char
                layer->function[DEBUG_CODE( func_idx = ) *(start + 1)] = function;

                break;
        }

        case '0': {
                if (*(start + 1) == 'x') {
                        // Base 16
                        layer->function[DEBUG_CODE( func_idx = ) strtol(start, &i, 16)] = function;

                        break;
                }
                
                // Base 8
                layer->function[DEBUG_CODE( func_idx = ) strtol(start, &i, 8)] = function;
                
                break;
        }

        default: {
                // Base 10
                layer->function[DEBUG_CODE( func_idx = ) strtol(start, &i, 10)] = function;
        }
        }

        DEBUG_MSG("Pointed function 0x%X to pointer 0x%X (LD: 0x%X) on layer %d\n", func_idx, function, layer_down, layer->level);
}

// Initialize the keyboard handler, import
// key combbinations from the configuration
// file into a data structure.
void init_keyboard(char *line) {
        // "keyseq func:0x10,2,'a','b','c'" 
        // Series of ascii codes, hex, decimal, or ascii.

        if (strncmp(line, "keyseq", 6) != 0)
                return;

        DEBUG_MSG("Received \"%s\"\n", line);


        // Error: Whitespace is factored into the function name.
        //        Function name should be completely free of white-
        //        space.
        int i = 0;
        line += 7;
        for (; (i < strlen(line)) && ((*(line + i) != ':') && !isblank(*(line + i))); i++);
        char *function_name = (char *)(malloc(i + 1));
        memset(function_name, 0, i + 1);
        strncpy(function_name, line, i);

        DEBUG_MSG("\"%s\": %d \"%s\" (%lu)\n", line, i, function_name, general_hash(function_name));

        DEBUG_CODE( top_layer.level = 0; )

        i++;
        create_path((line + i), (line + strlen(line) - 1), functions[GET_FUNC_IDX(function_name)], &top_layer);

        free(function_name);
}