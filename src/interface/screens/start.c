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
#include <interface/screens/start.h>
#include <interface/interface.h>
#include <ncurses.h>

float time = 0;

void render(struct render_context *context) {
        for (int i = 0; i < max_y; i++) {
                for (int j = 0; j < max_x; j++) {
                        int value = i * j * time;
                        int x = (value + j) % max_x;
                        int y = (value + i) % max_y;

                        char c = (value <= 0xFF) ? 'A' : ' ';

                        mvprintw(y, x, "%c", c);
                }
        }

        mvprintw(0, 0, "%f", time);

        time += 0.001;

        if (time >= 10) time = 5;

        move(max_y / 2 - 1, max_x / 2 - 1);
}

void update(struct render_context *context) {
        
}

void register_start() {
        register_screen("start", render, update);
}