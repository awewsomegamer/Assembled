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
};
struct key_layer top_layer;

struct key_stack_element {
        char key;
        time_t time;
};
struct key_stack_element key_stack[MAX_KEY_ELEMENTS];
int sp = 0;

// Collapse (interpet) current key stack, looking
// for any valid combinations of keys.
// TODO: Ensure the stack won't stick together
//       and lock the user from enter more keys.
//       Possible fix: make the stack bigger.
void collapse_stack() {
        struct key_layer *layer = &top_layer;
        for (int i = 0; i < sp; i++) {
                void (*func_ptr)() = layer->function[key_stack[i].key];

                if (func_ptr == NULL) {
                        break;
                }

                if (func_ptr == layer_down) {
                        layer = layer->next;
                        continue;
                }

                (func_ptr)();
                sp = 0;
        }
}

// Acknowledge a key, put it on the stack, ask
// to collapse the stack.
void key(char c) {
        key_stack[sp].key = c;
        key_stack[sp++].time = time(NULL);
        collapse_stack();
}

void create_path(char *start, char *end, void (*function)(), struct key_layer *layer) {    
        char *i;
        for (i = start; i < end; i++) {
                if (*i == ',') {
                        if (layer->next == NULL)
                                layer->next = (struct key_layer *)malloc(sizeof(struct key_layer));

                        create_path(i + 1, end, function, layer->next);
                        function = layer_down;
                }
        }

        switch (*start) {
        case '\'': {
                // Char
                layer->function[*(start + 1)] = function;

                break;
        }

        case '0': {
                if (*(start + 1) == 'x') {
                        // Base 16
                        layer->function[strtol(start, &i, 16)] = function;

                        break;
                }
                
                // Base 8
                layer->function[strtol(start, &i, 8)] = function;
                
                break;
        }

        default: {
                // Base 10
                layer->function[strtol(start, &i, 10)] = function;
        }
        }


}

// Initialize the keyboard handler, import
// key combbinations from the configuration
// file into a data structure.
void init_keyboard(char *line) {
        // "keyseq func:0x10,2,'a','b','c'" 
        // Series of ascii codes, hex, decimal, or ascii.

        if (strncmp(line, "keyseq", 6) != 0)
                return;

        int i = 0;
        for (; i < strlen(line) && ((*(line + 7 + i)) != ':'); i++) printw("%c ", (*(line + 7 + i)));
        char *function_name = (char *)(malloc(i));
        strncpy(function_name, line + 7, i);

        i++;
        create_path((line + 7 + i), (line + strlen(line) - 1), functions[GET_FUNC_IDX(function_name)], &top_layer);
}