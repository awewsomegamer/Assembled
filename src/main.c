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
#include <signal.h>

#include <global.h>
#include <editor/keyboard.h>
#include <editor/config.h>
#include <interface/interface.h>
#include <interface/screens/start.h>
#include <util.h>

#include <editor/functions.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

bool running = 1;
int currently_active_screen = 0;

void init_ncurses() {
        setlocale(LC_ALL, "UTF-8");

        initscr();
        cbreak();
        noecho();
        nonl();
        
        intrflush(stdscr, FALSE);
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);

        if (has_colors()) {
                start_color();
                use_default_colors();

                init_pair(ASSEMBLED_COLOR_HIGHLIGHT, COLOR_BLACK, COLOR_WHITE);
        }

        getmaxyx(stdscr, current_render_context.max_y, current_render_context.max_y);
}

void editor() {
        // W & D move  cursor successfully, any other key
        // freezes everything.
        int c = getch();
        
        if (c > -1)
                key(c);

        if (active_screen != NULL)
                active_screen->update(&current_render_context);
}

void interface() {
        erase();

        if (active_screen != NULL)
                active_screen->render(&current_render_context);
        
        refresh();
}

void terminate(int signal) {
        running = 0;
}

int main(int argc, char **argv) {
        read_config();
        register_start();
        switch_to_screen("start");

        signal(SIGINT,terminate);

        init_ncurses();

        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);

        double now = 0;
        double last = spec.tv_nsec / 1000000000.0;
        double delta = 0;
        double accumulator = 0;

        time(NULL);

        while (running) {
                bool render = 0;

                clock_gettime(CLOCK_REALTIME, &spec);
                now = spec.tv_nsec / 1000000000.0;
                delta = now - last;
                last = now;
                accumulator += fabs(delta);

                while (accumulator >= TARGET_FPS) {
                        editor();

                        accumulator -= TARGET_FPS;
                        render = 1;
                }

                usleep(1000);

                if (render) {
                        interface();
                }
        }

        endwin();

        return 0;
}