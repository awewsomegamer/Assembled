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

#include <bits/time.h>
#include <curses.h>
#include <ncurses.h>
#include <locale.h>

#include <global.h>
#include <editor/keyboard.h>
#include <interface/interface.h>
#include <interface/screens/start.h>

#include <editor/functions.h>
#include <time.h>
#include <unistd.h>

bool running = 1;
int currently_active_screen = 0;

int max_x = 0;
int max_y = 0;

void init_ncurses() {
        setlocale(LC_ALL, "UTF-8");        
        initscr();
        cbreak();
        noecho();
        nonl();
        intrflush(stdscr, FALSE);
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);

        getmaxyx(stdscr, max_y, max_x);
}

void editor() {
        char c = getch();
        
        if (c > 0)
                key(c);
}

void interface() {
        if (active_screen != NULL)
                active_screen->render(&current_render_context);
        
        refresh();
}

int main(int argc, char **argv) {
        init_ncurses();

        register_start();
        switch_to_screen("start");

        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);

        double now = 0;
        double last = spec.tv_nsec / 1000000000.0;
        double delta = 0;
        double accumulator = 0;

        while (running) {
                uint8_t render = 0;

                clock_gettime(CLOCK_REALTIME, &spec);
                now = spec.tv_nsec / 1000000000.0;
                delta = now - last;
                last = now;
                accumulator += delta;

                while (accumulator >= TARGET_FPS) {
                        editor();

                        accumulator -= TARGET_FPS;
                        render = 1;
                }

                usleep(1);

                if (render) {
                        interface();
                }
        }

        endwin();

        return 0;
}