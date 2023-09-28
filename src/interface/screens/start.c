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

#include "global.h"
#include <curses.h>
#include <interface/screens/start.h>
#include <interface/interface.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

#define CHARS_FROM_CENTER 20

float t = 0.01;
uint8_t dir = 0;

const char *menu_table_left[] = {
        "Hello",
        "World this",
        "Is"
};

const char *menu_table_right[] = {
        "A",
        "Test of",
        "My",
        "Menu system"
};

void render(struct render_context *context) {
        for (int i = 0; i < max_y; i++) {
                for (int j = 0; j < max_x; j++) {
                        int value = i * j * t * t;
                        int x = (value + j * i) % max_x;
                        int y = (value + i * j) % max_y;
                        
                        char c = (value <= 0xFF) ? ('0' + (x % 2)) : ' ';
                        
                        mvprintw(y, x, "%c", c);
                }
        }

        // Draw Logo
        

        // Draw Options Menu
        for (int i = 0; i < sizeof(menu_table_left)/sizeof(menu_table_left[0]); i++) { 
                mvprintw((max_y / 2 + i), (max_x / 2) - strlen(menu_table_left[i]) - CHARS_FROM_CENTER, "%s", menu_table_left[i]);
        }

        for (int i = 0; i < sizeof(menu_table_right)/sizeof(menu_table_right[0]); i++) { 
                mvprintw((max_y / 2 + i), (max_x / 2) - strlen(menu_table_right[i]) + CHARS_FROM_CENTER, "%s", menu_table_right[i]);
        }
}

void update(struct render_context *context) {
        t += (dir ? -0.00005 : 0.00005);

        if (t >= 0.079) // 0.078
                dir = 1;
        if (t <= 0.01)
                dir = 0;
}

void register_start() {
        register_screen("start", render, update);
}