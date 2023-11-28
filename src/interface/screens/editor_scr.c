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

#include <editor/buffer/buffer.h>
#include <editor/buffer/editor.h>
#include <editor/config.h>
#include <global.h>
#include <interface/interface.h>
#include <curses.h>
#include <interface/screens/editor_scr.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CURSOR_X (active_text_file->active_buffer->cx)
#define CURSOR_Y (active_text_file->cy)

static int line_length = 0;

static int differential = 0;
static int offset = 0;

static bool selection = 0;

char editor_scr_message[1024];

static void render(struct render_context *context) {
	struct column_descriptor descriptor = column_descriptors[current_column_descriptor];
	struct text_buffer *active_buffer = active_text_file->active_buffer;

	// Create and initialize a list of current pointers
	struct line_list_element **currents = (struct line_list_element **)calloc(descriptor.column_count, sizeof(struct line_list_element *));

	for (int i = 0; i < descriptor.column_count; i++) {
		currents[i] = active_text_file->buffers[i]->head;

		for (int j = 0; j < offset; j++) {
			currents[i] = currents[i]->next;
		}
	}

	// Variables for controlling the y-axis
	int cy_wrap_distortion = 0;
	int element_wrap_distortion = 0;
	int y = 0;

	// Restrict cx to the end of the current line
	line_length = strlen(active_buffer->current_element->contents);

	if (CURSOR_X > line_length) {
		CURSOR_X = line_length;
	}

	// While you can read a line
	while (currents[0] != NULL) {
		// Variables by which the above change
		int applied_cy_distortion = 0;
		int applied_element_distortion = 0;

		for (int i = 0; i < descriptor.column_count; i++) {
			struct line_list_element *current = currents[i];

			// Calculate the number of characters the current column
			// can fit
			int max_length = context->max_x - descriptor.column_positions[i];

			// Special case for the last column (max_x - column_posistion)
			if (i + 1 < descriptor.column_count) {
				max_length = descriptor.column_positions[i + 1] - descriptor.column_positions[i];
			}

			// Calculate the number of lines that will wrap
			int distortion = strlen(current->contents) / max_length;

			// If the current distortion is larger than the one
			// that will be applied at the end of the loop,
			// change it
			if (distortion > applied_element_distortion) {
				applied_element_distortion = distortion;
			}

			// If the cursor is under the distorted line and the
			// current distortion is larger than the applied distortion,
			// change it
			if (CURSOR_Y - offset > y && (distortion > applied_cy_distortion)) {
				applied_cy_distortion = distortion;
			}

			// Selection rendering
			struct bound *start = &active_buffer->selection_start;
			struct bound end_v = { .x = CURSOR_X, .y = CURSOR_Y };
			struct bound *end = &end_v;

			if (start->y > end->y) {
				struct bound *tmp = start;
				start = end;
				end = tmp;
			}

			if (start->x > end->x) {
				struct bound *tmp = start;
				start = end;
				end = tmp;
			}

			// The amount of characters needed to be filled in
			int char_mode = 0;
			int tmp1 = y + offset;

			if (tmp1 == start->y) {
			 	char_mode = -start->x;
			} else if (tmp1 == end->y) {
			 	char_mode = end->x;
			}

			// Draw each line of the string
			for (int x = 0; x < strlen(current->contents); x += max_length) {
				if (tmp1 == start->y && char_mode == 0 && selection == 1) {
				 	attron(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));
				} else if (tmp1 == end->y && char_mode == 0 || selection == 0) {
					attroff(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));
				}

				if (char_mode != 0) {
				 	int l = 0;

				 	for (; l < abs(char_mode); l++) {
				 		mvaddch(y + (x / max_length) + element_wrap_distortion, descriptor.column_positions[i] + l, *(current->contents + x + l));
				 	}

				 	if (char_mode > 0) {
				 		attroff(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));
				 	} else if (char_mode < 0) {
				 		attron(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));
				 	}

					// ERROR: Adds additional characters on wrapped lines
				 	for (; l < min(max_length, strlen(current->contents)); l++) {
				 		mvaddch(y + (x / max_length) + element_wrap_distortion, descriptor.column_positions[i] + l, *(current->contents + x + l));
				 	}

				 	continue;
				}

				mvprintw(y + (x / max_length) + element_wrap_distortion, descriptor.column_positions[i], "%.*s", max_length, (current->contents + x));
			}

			// Move onto the next line
			currents[i] = currents[i]->next;
		}

		// Update distortions
		cy_wrap_distortion += applied_cy_distortion;
		element_wrap_distortion += applied_element_distortion;

		// Move to the next line index
		y++;
	}

	// Print information
	mvprintw(context->max_y - 1, 0, "EDITING (%d, %d) %s", CURSOR_Y + 1, CURSOR_X + 1, editor_scr_message);


	// Position the cursor appropriately
	int column_start = active_buffer->col_start;
	int column_length = (active_buffer->col_end == -1 ? context->max_x : active_buffer->col_end) - column_start;

	move(CURSOR_Y - offset + cy_wrap_distortion + (CURSOR_X / column_length), (CURSOR_X % column_length) + column_start);
}

static void update(struct render_context *context) {
	// The differential is checked, if it has overflowed
	// or underflowed the screen, restrict it, and update
	// the offset
	if (differential > context->max_y - 2) {
		differential = context->max_y - 2;
		offset++;
	} else if (differential < 0) {
		differential = 0;
		offset = max(--offset, 0);
	}
}

// TODO: Scrolling sometimes gets stuck
static void local(int code, int value) {
	struct column_descriptor descriptor = column_descriptors[current_column_descriptor];

	switch (code) {
	case LOCAL_ARROW_UP: {
		bool moved = 0;

		// Update all current pointers one up
		for (int i = 0; i < descriptor.column_count; i++) {
			struct line_list_element *current = active_text_file->buffers[i]->current_element;
			
			if (current->prev != NULL) {
				active_text_file->buffers[i]->current_element = current->prev;
				moved = 1;
			}
		}

		// If the above managed to move the pointers
		// up, we can move the cursor
		if (CURSOR_Y > 0 && moved) {
			CURSOR_Y--;
			differential--;
		}

		break;
	}

	case LOCAL_ARROW_DOWN: {
		bool moved = 0;

		// Update all current pointers one down
		for (int i = 0; i < descriptor.column_count; i++) {
			struct line_list_element *current = active_text_file->buffers[i]->current_element;
		
			if (current->next != NULL) {
				active_text_file->buffers[i]->current_element = current->next;
				moved = 1;
			}
		}

		// If the above managed to move the pointers
		// down, we can move the cursor
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
	
	case LOCAL_ENTER: {
		buffer_char_insert('\n');
	}

	case LOCAL_LINE_INSERT: {
		differential++;

		break;
	}

	case LOCAL_LINE_DELETION: {
		differential--;

		break;
	}
	
	case LOCAL_BUFFER_LEFT: {
		int i = active_text_file->active_buffer_idx;

		if (i - 1 >= 0) {
			active_text_file->active_buffer = active_text_file->buffers[i - 1];
			(active_text_file->active_buffer_idx)--;
		}

		break;
	}

	case LOCAL_BUFFER_RIGHT: {
		int i = active_text_file->active_buffer_idx;

		if (i + 1 < column_descriptors[current_column_descriptor].column_count) { 
			active_text_file->active_buffer = active_text_file->buffers[i + 1];
			(active_text_file->active_buffer_idx)++;
		}

		break;
	}

	case LOCAL_BUFFER_CHAR: {
		if (value == '\b' || value == 263) {
			buffer_char_del();
			break;
		}

		buffer_char_insert(value);

		break;
	}

	case LOCAL_WINDOW_LEFT: {
		int i = min(active_text_file_idx - 1, 0);

		for (; i >= 0; i--) {
			if (text_files[i] != NULL) {
				break;
			}
		}

		if (text_files[i] != NULL) {
			active_text_file = text_files[i];
			active_text_file_idx = i;

			sprintf(editor_scr_message, "SWITCHED TO %s", active_text_file->name);
		} else {
			sprintf(editor_scr_message, "BEGIN");
		}

		break;
	}

	case LOCAL_WINDOW_RIGHT: {
		int i = min(active_text_file_idx + 1, MAX_TEXT_FILES - 1);

		for (; i < MAX_TEXT_FILES; i++) {
			if (text_files[i] != NULL) {
				break;
			}
		}

		if (text_files[i] != NULL && i < MAX_TEXT_FILES) {
			active_text_file = text_files[i];
			active_text_file_idx = i;

			sprintf(editor_scr_message, "SWITCHED TO %s %d", active_text_file->name, i);
		} else {
			sprintf(editor_scr_message, "END");
		}
		
		break;
	}

	case LOCAL_WINDOW_SELECTION: {
		selection = !selection;

		if (selection) {
			active_text_file->active_buffer->selection_start.x = active_text_file->active_buffer->cx;
			active_text_file->active_buffer->selection_start.y = active_text_file->cy;
		}

		break;
	}
	}
}

void register_editor_screen() {
	DEBUG_MSG("Registering editor screen\n");

		int i = register_screen("editor", render, update, local);
		screens[i].render_options |= SCR_OPT_ON_UPDATE;
}

struct cfg_token *configure_editor_screen(struct cfg_token *token) {
	return token;
}
