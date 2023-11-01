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

#include <interface/screens/editor_scr.h>
#include "interface/theming/themes.h"
#include <bits/time.h>
#include <curses.h>
#include <ncurses.h>
#include <locale.h>
#include <signal.h>

#include <global.h>
#include <editor/buffer/editor.h>
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
FILE *debug_log_file = NULL;

void init_ncurses() {
        DEBUG_MSG("Initializing ncurses\n");
        setlocale(LC_ALL, "UTF-8");

        initscr();
        cbreak();
        noecho();
        
        intrflush(stdscr, FALSE);
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);

        if (has_colors()) {
                DEBUG_MSG("Terminal has colors\n");
                start_color();
                use_default_colors();
                register_custom_colors();

                init_pair(ASSEMBLED_COLOR_HIGHLIGHT, COLOR_BLACK, 16);
                DEBUG_MSG("Initialized Assembled colors\n");
        }

        int x = 0, y = 0;
        getmaxyx(stdscr, y, x);
        
        current_render_context.max_x = x;
        current_render_context.max_y = y;

        if (current_render_context.max_x == 0 || current_render_context.max_y == 0) {
                endwin();
                printf("Max X: %d, Max Y: %d are unsuitable for operation\n", current_render_context.max_x, current_render_context.max_y);
                exit(1);
        }
        
        DEBUG_MSG("Initialized ncurses\n");
}

void editor() {
        // W & D move  cursor successfully, any other key
        // freezes everything.
        int c = getch();
        
        if (c > -1)
                key(c);

        if (active_screen != NULL) {
                active_screen->update(&current_render_context);
        }
}

void interface() {
        erase();

        if (active_screen != NULL) {
                active_screen->render(&current_render_context);
        }
        
        refresh();
}

void terminate(int signal) {
        running = 0;
}

int main(int argc, char **argv) {
        DEBUG_CODE( debug_log_file = fopen("debug.log", "w"); )

        read_config();
        register_start_screen();
        register_editor_screen();

        if (argc > 1) {
                save_file(load_file(argv[1]));
                switch_to_screen("editor");
        } else {
                switch_to_screen("start");
        }

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

        DEBUG_MSG("Successfuly exited\n");

        DEBUG_CODE( fclose(debug_log_file); )

        return 0;
}
