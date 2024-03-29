/**
 * @file main.c
 * @author awewsomegamer <awewsomegamer@gmail.com>
 *
 * @section LICENSE
 *
 * Assembled - Column based text editor
 * Copyright (C) 2023-2024 awewsomegamer
 *
 * This file is apart of Assembled.
 *
 * Assembled is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @section DESCRIPTION
 *
 * The main program file.
*/

#include <editor/syntax/syntax.h>
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
#include <includes.h>

/**
 * Controls whether the main loop is running (1) or not (0).
 * */
bool running = 1;
/**
 * Set when new input from the keyboard is present, otherwise 0.
 * */
bool update = 0;
/**
 * Debug log file pointer.
 *
 * This file is only used if Assembled is compiled with
 * AS_DEBUG_ENABLE defined. This can be acheived through
 * adding the `debug` target to the `make` command.
 * `make debug`
 * */
FILE *__AS_DBG_LOG_FILE__ = NULL;

static int default_column_definition[] = { 0 };

struct AS_GlobalCtx as_ctx = { 0 };
/**
 * Initialize ncurses library
 *
 * Initialize the terminal.
 * */
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

/**
 * Update the state of the editor
 *
 * Checks if the keyboard has any new input and
 * updates the current screen.
 * */
void editor() {
	// Read in current key
        int c = getch();

        if (c > -1) {
		// If a key is present, cleaar the message,
		// trigger an update and tell the keyboard to
		// handle the key
                sprintf(as_ctx.editor_scr_message, "");
                update = 1;

                key(c);
        }

	// Udate the screen if it exists and has an update function
        if (as_ctx.screen != NULL && as_ctx.screen->update != NULL) {
                as_ctx.screen->update(&as_ctx.render_ctx);
        }
}

/**
 * Render the current screen
 *
 * Render the current active screen, clearing
 * the screen in the process
 * */
void interface() {
        erase();

	// Draw the screen if it exists and has a render function
        if (as_ctx.screen != NULL && as_ctx.screen->render != NULL) {
                as_ctx.screen->render(&as_ctx.render_ctx);
        }
        
        refresh();
}

/**
 * Function to be called when SIGINT is received.
 * */
void terminate(int signal) {
        running = 0;
}

int main(int argc, char **argv) {
	// Setup up debug file
        AS_DEBUG_CODE( 
                __AS_DBG_LOG_FILE__ = fopen("debug.log", "w");
                if (__AS_DBG_LOG_FILE__ == NULL) {
                        printf("Failed to open ./debug.log\n");

                        return 1;
                }
        )

	// Initialize
	as_ctx.col_desc_i = -1;

        read_config();
	init_syntax();

	if (as_ctx.col_desc_i == -1) {
		// No column was selected to be booted with, try to find a defined one
		for (int i = 0; i < AS_MAX_COLUMNS; i++) {
			if (as_ctx.col_descs[i].column_positions != NULL) {
				as_ctx.col_desc_i = i;
				break;
			}
		}
	}

        if (as_ctx.col_desc_i == -1) {
		// No predefined column was found, define a single column
                printf("Failed to find a user defined column, defining a single column\n");
                AS_DEBUG_MSG("Failed to find a user defined column, defining a single column\n");

                as_ctx.col_desc_i = 0;
                as_ctx.col_descs[as_ctx.col_desc_i].column_positions = default_column_definition;
                as_ctx.col_descs[as_ctx.col_desc_i].column_count = 1;
                as_ctx.col_descs[as_ctx.col_desc_i].delimiter = 0;
        }

	// Register screens
        register_start_screen();
        register_editor_screen();
        register_file_load_scr();

	// Check for file to load
        if (argc > 1) {
                load_file(argv[1]);
                switch_to_screen("editor");
        } else {
                switch_to_screen("start");
        }

	// Allow program to terminate properly on Ctrl + C
        signal(SIGINT, terminate);

        init_ncurses();

	// Initialization is complete
	// Get to running

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
				// No update, don't need to render
                                accumulator = 0;
                                break;
                        }

                        accumulator -= TARGET_FPS;
                        render = 1;
                }

		// Probably impossible to type a character in an
		// interval below ~8 milliseconds
                usleep(8000);

                if (render || (as_ctx.screen->render_options & SCR_OPT_ALWAYS)) {
			interface();
                }

                update = 0;
        }

	// Shutdown
	// Stop ncurses window
        endwin();

        AS_DEBUG_MSG("Successfuly exited\n");

	// Close debug file
        AS_DEBUG_CODE( fclose(__AS_DBG_LOG_FILE__); )

        return 0;
}
