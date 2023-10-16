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

// ERROR: Thrown into an infinite loop, when 15 or more characters are involved in a path
void create_path(char *start, char *end, void (*function)(), struct key_layer *layer) {
        char *i = start;

        // Step through the current string to the end
        for (; i < end; i++) {
                // While we do not ecnounter a ',', we continue to the next char
                if (*i != ',')
                        continue;

                // We have encountered a ','
                // Create a new layer if one doesn't already exist
                if (layer->next == NULL) {
                        layer->next = (struct key_layer *)malloc(sizeof(struct key_layer));
                        key_layer_count++;

                        DEBUG_CODE( layer->next->level = layer->level + 1; )
                        memset(layer->next, 0, sizeof(struct key_layer));
                }
                
                // Recurse down
                create_path(i + 1, end, function, layer->next);

                // Once we get back here, our function is no longer going
                // to be the one originally set, rather it is going to
                // be layer down
                function = layer_down;
        }

        // Have now processed every character after us

        // Skip over any whitespace
        for (; (isblank(*start)) && (start < end); start++)
                ;

        DEBUG_CODE( int func_idx = 0; )

        // Interpret ASCII code
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
void configure_keyboard(char *line) {
        // "keyseq func:0x10,2,'a','b','c'" 
        // Series of ASCII codes, hex, decimal, or ascii.

        // Currently, only keyseqs are supported.
        if (strncmp(line, "keyseq", 6) != 0)
                return;

        DEBUG_MSG("Received \"%s\"\n", line);

        // Increment the line pointer
        line += 7;

        // Find the index of the colon, there can be no whitespace here
        int idx = read_line_section(line, ':');

        // If we discovered whitespace, or another error, report it
        if (idx == -1) {
                printf("Error encountered in \"%s\"\n", line);
                DEBUG_MSG("Error encountered in \"%s\"\n", line);
                exit(1);
        }

        // Use the index gathered above, and isolate the function name
        char *function_name = (char *)(malloc(idx + 1));
        memset(function_name, 0, idx + 1);
        strncpy(function_name, line, idx);
        
        DEBUG_MSG("\"%s\": %d \"%s\" (%lu)\n", line, idx, function_name, general_hash(function_name));

        DEBUG_CODE( top_layer.level = 0; )

        // Create a "petrified lightning" type path
        create_path((line + idx + 1), (line + strlen(line) - 1), functions[GET_FUNC_IDX(function_name)], &top_layer);

        // Memory Manage
        free(function_name);

        // Check if stack might stick
        if (key_layer_count >= MAX_KEY_ELEMENTS) {
                printf("Stack will stick! Longest keyseq: %d, stack length: %d\n", key_layer_count, MAX_KEY_ELEMENTS);
                DEBUG_MSG("Stack will stick! Longest keyseq: %d, stack length: %d\n", key_layer_count, MAX_KEY_ELEMENTS);
        }
}