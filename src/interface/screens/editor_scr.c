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

static int differential = 0;
static int offset = 0;

static void render(struct render_context *context) {
	struct column_descriptor descriptor = column_descriptors[current_column_descriptor];

        for (int i = 0; i < descriptor.column_count; i++) {
                struct line_list_element *current = active_text_file->buffers[i]->head;

		for (int j = 0; j < offset; j++) {
			current = current->next;
		}

                int y = 0;
		int max_length = context->max_x - descriptor.column_positions[i];

		if (i + 1 < descriptor.column_count) {
			max_length = descriptor.column_positions[i + 1] - descriptor.column_positions[i];
		}

                while (current != NULL && y < context->max_y - 1) {
			mvprintw(y++, descriptor.column_positions[i], "%.*s", max_length, current->contents);

                        current = current->next;
                }

        }

	mvprintw(context->max_y - 1, 0, "EDITING (%d, %d)", CURSOR_Y + 1, CURSOR_X + 1);

	line_length = strlen(active_text_file->active_buffer->current_element->contents);

	if (CURSOR_X > line_length) {
		CURSOR_X = line_length;
	}

        move(CURSOR_Y - offset, CURSOR_X + descriptor.column_positions[active_text_file->active_buffer_idx]);
}

static void update(struct render_context *context) {
	if (differential > context->max_y - 2) {
		differential = context->max_y - 2;
		offset++;
	} else if (differential < 0) {
		differential = 0;
		offset = max(--offset, 0);
	}
}

// TODO: Scrolling is broken, can't go up
static void local(int code) {
	struct column_descriptor descriptor = column_descriptors[current_column_descriptor];

        switch (code) {
        case LOCAL_ARROW_UP: {
		bool moved = 0;

		for (int i = 0; i < descriptor.column_count; i++) {
        		struct line_list_element *current = active_text_file->buffers[i]->current_element;
			
			if (current->prev != NULL) {
				active_text_file->buffers[i]->current_element = current->prev;
				moved = 1;
			}
		}

		if (CURSOR_Y > 0 && moved) {
			CURSOR_Y--;
			differential--;
		}

                break;
        }

        case LOCAL_ARROW_DOWN: {
		bool moved = 0;

                for (int i = 0; i < descriptor.column_count; i++) {
        		struct line_list_element *current = active_text_file->buffers[i]->current_element;
			
			if (current->next != NULL) {
				active_text_file->buffers[i]->current_element = current->next;
				moved = 1;
			}
		}

		if (moved) {
			CURSOR_Y++;
			differential++;
		}

		break;
        }

        case LOCAL_ARROW_LEFT: {
                if (CURSOR_X > 0) {
                        CURSOR_X--;
		}

                break;
        }

        case LOCAL_ARROW_RIGHT: {
                if (CURSOR_X < line_length) {
                        CURSOR_X++;
		}

                break;
        }
	
	case LOCAL_LINE_INSERT: {
		differential++;

		break;
	}

	case LOCAL_LINE_DELETION: {
		differential--;

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