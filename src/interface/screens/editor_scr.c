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

#include "editor/buffer/buffer.h"
#include "editor/buffer/editor.h"
#include "global.h"
#include "interface/interface.h"
#include <curses.h>
#include <interface/screens/editor_scr.h>
#include <string.h>
#include <unistd.h>

#define LEFT_MARGIN 5

static int line_length = 0;

static void render(struct render_context *context) {

        for (int i = 0; i < column_descriptors[0].column_count; i++) {
                struct line_list_element *current = active_text_file->buffers[i]->head;
                int y = 0;

                while (current != NULL && y < context->max_y) {
                        // mvprintw(y, 0, "%d", current->line);
                        // mvaddch(y, LEFT_MARGIN - 1, '|');
                        // mvprintw(y, LEFT_MARGIN, "%s", current->contents);
                        
                        mvprintw(y, column_descriptors[0].column_positions[i], "%s", current->contents);
                        y++;

                        // if (current->line == CURSOR_Y + 1) {
                        //         line_length = strlen(current->contents);
                        // }

                        current = current->next;
                }

                // if (CURSOR_X > line_length) {
                //         CURSOR_X = line_length;
                // }


        }
        // move(CURSOR_Y, CURSOR_X + LEFT_MARGIN);
}

static void update(struct render_context *context) {

}

static void local(int code) {
        // switch (code) {
        // case LOCAL_ARROW_UP: {
        //         CURSOR_Y--;

        //         break;
        // }

        // case LOCAL_ARROW_DOWN: {
        //         CURSOR_Y++;

        //         break;
        // }

        // case LOCAL_ARROW_LEFT: {
        //         if (CURSOR_X > 0)
        //                 CURSOR_X--;

        //         break;
        // }

        // case LOCAL_ARROW_RIGHT: {
        //         if (CURSOR_X < line_length)
        //                 CURSOR_X++;

        //         break;
        // }
        // }
}

void register_editor_screen() {
        DEBUG_MSG("Registering editor screen\n");
        register_screen("editor", render, update, local);
}

struct cfg_token *configure_editor_screen(struct cfg_token *token) {
        return token;
}