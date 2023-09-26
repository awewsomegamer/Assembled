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

#include <time.h>

#include <global.h>
#include <editor/keyboard.h>

struct key_stack_element {
        char key;
        time_t time;
};
struct key_stack_element key_stack[MAX_KEY_ELEMENTS];
int sp = 0;

// Collapse (interpet) current key stack, looking
// for any valid combinations of keys.
void collapse_stack() {
        if (key_stack[sp - 1].key > 32) {
                mvprintw(0, 0, "Info %d", sp);
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

// Initialize the keyboard handler, import
// key combbinations from the configuration
// file into a data structure.
void init_keyboard() {
        
}