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

#include <editor/buffer/editor.h>
#include <editor/keyboard.h>
#include <editor/config.h>

#include <interface/screens/editor_scr.h>
#include <interface/screens/file_load_scr.h>
#include <interface/theming/themes.h>
#include <interface/interface.h>
#include <interface/screens/start.h>

#include <global.h>
#include <util.h>

bool running = 1;
bool update = 0;
int currently_active_screen = 0;
FILE *__AS_DBG_LOG_FILE__ = NULL;

static int default_column_definition[] = { 0 };

struct AS_GlobalCtx as_ctx = { 0 };

void init_ncurses() {
        AS_DEBUG_MSG("Initializing ncurses\n");
        setlocale(LC_ALL, "UTF-8");

        initscr();
        cbreak();
        noecho();
        
        intrflush(stdscr, FALSE);
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);

        if (has_colors()) {
                AS_DEBUG_MSG("Terminal has colors\n");
                start_color();
                use_default_colors();
                register_custom_colors();

                init_pair(AS_COLOR_HIGHLIGHT, COLOR_BLACK, 16);
                AS_DEBUG_MSG("Initialized Assembled colors\n");
        }

        int x = 0, y = 0;
        getmaxyx(stdscr, y, x);
        
        as_ctx.render_ctx.max_x = x;
        as_ctx.render_ctx.max_y = y;

        if (as_ctx.render_ctx.max_x == 0 || as_ctx.render_ctx.max_y == 0) {
                endwin();
                printf("Max X: %d, Max Y: %d are unsuitable for operation\n", as_ctx.render_ctx.max_x, as_ctx.render_ctx.max_y);
                exit(1);
        }
        
        AS_DEBUG_MSG("Initialized ncurses\n");
}

void editor() {
        int c = getch();

        if (c > -1) {
                sprintf(as_ctx.editor_scr_message, "");
                update = 1;

                key(c);
        }

        if (as_ctx.screen != NULL && as_ctx.screen->update != NULL) {
                as_ctx.screen->update(&as_ctx.render_ctx);
        }
}

void interface() {
        erase();

        if (as_ctx.screen != NULL && as_ctx.screen->render != NULL) {
                as_ctx.screen->render(&as_ctx.render_ctx);
        }
        
        refresh();
}

void terminate(int signal) {
        save_all();

        running = 0;
}

int main(int argc, char **argv) {
        AS_DEBUG_CODE( 
                __AS_DBG_LOG_FILE__ = fopen("debug.log", "w");
                if (__AS_DBG_LOG_FILE__ == NULL) {
                        printf("Failed to open ./debug.log\n");

                        return 1;
                }
        )

	as_ctx.col_desc_i = -1;

        read_config();

	if (as_ctx.col_desc_i == -1) {
		for (int i = 0; i < MAX_COLUMNS; i++) {
			if (as_ctx.col_descs[i].column_positions != NULL) {
				as_ctx.col_desc_i = i;
				break;
			}
		}
	}

        if (as_ctx.col_desc_i == -1) {
                printf("Failed to find a user defined column, defining a single column\n");
                AS_DEBUG_MSG("Failed to find a user defined column, defining a single column\n");

                as_ctx.col_desc_i = 0;
                as_ctx.col_descs[as_ctx.col_desc_i].column_positions = default_column_definition;
                as_ctx.col_descs[as_ctx.col_desc_i].column_count = 1;
                as_ctx.col_descs[as_ctx.col_desc_i].delimiter = 0;
        }

        register_start_screen();
        register_editor_screen();
        register_file_load_scr();

        if (argc > 1) {
                load_file(argv[1]);
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

	// "Wake-up" cycle
	editor();
	interface();

        while (running) {
                bool render = 0;

                clock_gettime(CLOCK_REALTIME, &spec);
                now = spec.tv_nsec / 1000000000.0;
                delta = now - last;
                last = now;
                accumulator += fabs(delta);

                while (accumulator >= TARGET_FPS) {
                        editor();

                        if (update == 0) {
                                accumulator = 0;
                                break;
                        }

                        accumulator -= TARGET_FPS;
                        render = 1;
                }

                usleep(8000);

                if (render || (as_ctx.screen->render_options & SCR_OPT_ALWAYS)) {
                        interface();
                }

                update = 0;
        }

        endwin();

        AS_DEBUG_MSG("Successfuly exited\n");

        AS_DEBUG_CODE( fclose(__AS_DBG_LOG_FILE__); )

        return 0;
}
