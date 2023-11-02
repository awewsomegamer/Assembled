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

#define CURSOR_X (active_text_file->active_buffer->cx)
#define CURSOR_Y (active_text_file->cy)

static int line_length = 0;

static void render(struct render_context *context) {
        for (int i = 0; i < column_descriptors[0].column_count; i++) {
                struct line_list_element *current = active_text_file->buffers[i]->head;
                int y = 0;

                while (current != NULL && y < context->max_y - 1) {
			mvprintw(y, column_descriptors[0].column_positions[i], "%s", current->contents);
                        y++;

                        current = current->next;
                }

		mvprintw(context->max_y - 1, 0, "EDITING (%d, %d)", CURSOR_X + 1, CURSOR_Y + 1);

                if (CURSOR_X > strlen(active_text_file->active_buffer->current_element->contents)) {
                        CURSOR_X = line_length;
                }
        }

        move(CURSOR_Y, CURSOR_X);
}

static void update(struct render_context *context) {

}

static void local(int code) {
        struct line_list_element *current = active_text_file->active_buffer->current_element;

        switch (code) {
        case LOCAL_ARROW_UP: {
                if (current->prev != NULL) {
                        active_text_file->active_buffer->current_element = current->prev;
                        CURSOR_Y--;
                }

                break;
        }

        case LOCAL_ARROW_DOWN: {
                if (current->next != NULL) {
                        active_text_file->active_buffer->current_element = current->next;
                        CURSOR_Y++;
                }

                break;
        }

        case LOCAL_ARROW_LEFT: {
                if (CURSOR_X > 0)
                        CURSOR_X--;

                break;
        }

        case LOCAL_ARROW_RIGHT: {
                if (CURSOR_X < line_length)
                        CURSOR_X++;

                break;
        }
        }
}

void register_editor_screen() {
        DEBUG_MSG("Registering editor screen\n");
        register_screen("editor", render, update, local);
}

struct cfg_token *configure_editor_screen(struct cfg_token *token) {
        return token;
}