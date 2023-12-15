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

#include <interface/interface.h>
#include <interface/screens/editor_scr.h>

#include <global.h>
#include <stdio.h>

#define CURSOR_X (as_ctx.text_file->active_buffer->cx)
#define CURSOR_Y (as_ctx.text_file->cy)

static int line_length = 0;

static int differential = 0;
static int offset = 0;

static void render(struct AS_RenderCtx *context) {
	struct AS_ColDesc descriptor = as_ctx.col_descs[as_ctx.col_desc_i];
	struct AS_TextBuf *active_buffer = as_ctx.text_file->active_buffer;

	// Create and initialize a list of current pointers
	struct AS_LLElement **currents = (struct AS_LLElement **)calloc(descriptor.column_count, sizeof(struct AS_LLElement *));

	for (int i = 0; i < descriptor.column_count; i++) {
		currents[i] = as_ctx.text_file->buffers[i]->head;

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
			struct AS_LLElement *current = currents[i];
			struct AS_TextBuf *current_buffer = as_ctx.text_file->buffers[i];

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
			struct AS_Bound *start = &active_buffer->selection_start;
			struct AS_Bound end_v = { .x = CURSOR_X, .y = CURSOR_Y };
			struct AS_Bound *end = &end_v;

			// Ensure start < end
			if (start->y > end->y || start->x > end->x) {
				struct AS_Bound *tmp = start;
				start = end;
				end = tmp;
			}

			int true_y = y + offset;

			int char_mode = 0;
			bool selection_extreme = 0;
			bool extreme_side = 0;
			bool selection = current_buffer->selection_enabled;

			// Determine which extreme current line is on
			if (true_y == start->y) {
				char_mode = start->x;
				selection_extreme = 1;
				extreme_side = 0;
			} else if (true_y == end->y) {
				char_mode = end->x;
				selection_extreme = 1;
				extreme_side = 1;
			}

			// Draw each line of the string
			for (int x = 0; x < strlen(current->contents); x += max_length) {
				int yc = y + (x / max_length) + element_wrap_distortion;
				int xc = descriptor.column_positions[i];

				// Highlight contents if selected
				if (((start->y < true_y && end->y > true_y) || (start->y <= true_y && end->y >= true_y && as_ctx.text_file->selected_buffers != 0)) && selection) {
					attron(COLOR_PAIR(AS_COLOR_HIGHLIGHT));
				} else {
					attroff(COLOR_PAIR(AS_COLOR_HIGHLIGHT));
				}

				// Draw regular text
				if (selection == 0 || selection_extreme == 0 || as_ctx.text_file->selected_buffers != 0) {
					mvprintw(yc, xc, "%.*s", max_length, (current->contents + x));
					continue;
				}

				// Enable or disble highlighting for first segment
				if (extreme_side == 0) {
					attroff(COLOR_PAIR(AS_COLOR_HIGHLIGHT));
				} else if (char_mode > 0) {
					attron(COLOR_PAIR(AS_COLOR_HIGHLIGHT));
				}

				// Draw first segment
				int length = min(max_length, max(0, abs(char_mode) - x));
				mvprintw(yc, xc, "%.*s", length, (current->contents + x));

				// Disable or enable highlighting for second segment
				if (extreme_side == 0) {
					attron(COLOR_PAIR(AS_COLOR_HIGHLIGHT));
				} else if (char_mode > 0) {
					attroff(COLOR_PAIR(AS_COLOR_HIGHLIGHT));
				}

				// Draw second segment
				mvprintw(yc, xc + length, "%.*s", max_length - length, (current->contents + x + length));
			}

			attroff(COLOR_PAIR(AS_COLOR_HIGHLIGHT));

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
	mvprintw(context->max_y - 1, 0, "EDITING (%d, %d) %s", CURSOR_Y + 1, CURSOR_X + 1, as_ctx.editor_scr_message);

	// Position the cursor appropriately
	int column_start = active_buffer->col_start;
	int column_length = (active_buffer->col_end == -1 ? context->max_x : active_buffer->col_end) - column_start;

	move(CURSOR_Y - offset + cy_wrap_distortion + (CURSOR_X / column_length), (CURSOR_X % column_length) + column_start);
}

static void update(struct AS_RenderCtx *context) {
	// The differential is checked, if it has overflowed
	// or underflowed the screen, restrict it, and update
	// the offset
	if (differential >= context->max_y - 2) {
		differential = context->max_y - 3;
		offset++;
	} else if (differential < 0) {
		differential = 0;
		offset = max(--offset, 0);
	}
}

// TODO: Scrolling desynchronizes occasionally
static void local(int code, int value) {
	struct AS_ColDesc descriptor = as_ctx.col_descs[as_ctx.col_desc_i];

	switch (code) {
	// ERROR: The YMOVE and XMOVE can sometimes result
	//        in the cursor being locked out of bounds
	case LOCAL_ARROW_YMOVE: {
		bool moved = 0;

		// Update all current pointers one up
		for (int i = 0; i < descriptor.column_count; i++) {
			struct AS_LLElement *current = as_ctx.text_file->buffers[i]->current_element;

			if ((value == -1 ? current->prev : current->next) != NULL) {
				as_ctx.text_file->buffers[i]->current_element = (value == -1 ? current->prev : current->next);
				moved = 1;
			}
               }

		// If the above managed to move the pointers
		// up, we can move the cursor
		if (CURSOR_Y >= 0 && moved) {
			CURSOR_Y += value;
			differential += value;

			sprintf(as_ctx.editor_scr_message, "%s\n", (value == -1 ? "UP" : "DOWN"));
		}
		break;
	}

	case LOCAL_ARROW_XMOVE: {
		if (CURSOR_X >= 0 && CURSOR_X <= line_length) {
			CURSOR_X += value;
			CURSOR_X = max(CURSOR_X, 0);

			sprintf(as_ctx.editor_scr_message, "%s\n", (value == -1 ? "LEFT" : "RIGHT"));
		}

		break;
	}

	// LOCAL_ENTER
	case LOCAL_ENTER: {
		buffer_char_insert('\n');
	}

	case LOCAL_LINE_INSERT: {
		differential++;

		break;
	}
	// END OF LOCAL_ENTER

	case LOCAL_LINE_DELETION: {
		differential--;

		break;
	}

	case LOCAL_BUFFER_MOVE: {
		int i = as_ctx.text_file->active_buffer_idx;

		if (i + value >= 0 && i + value < descriptor.column_count) {
			// Can move to next buffer
			// Collect information
			struct AS_Bound start = as_ctx.text_file->active_buffer->selection_start;
			uint8_t selection = as_ctx.text_file->active_buffer->selection_enabled;

			// Check the if the direction of the move aligns with previous moves
			int selected_buffers_sign = ((as_ctx.text_file->selected_buffers) >> 31) & 1;
			int value_sign = ((value) >> 31) & 1;

			if (selected_buffers_sign != value_sign && as_ctx.text_file->selected_buffers != 0) {
				// User has changed direction, deselect current buffer
				as_ctx.text_file->active_buffer->selection_enabled = 0;
			}

			// Move to next buffer
			as_ctx.text_file->active_buffer = as_ctx.text_file->buffers[i + value];
			struct AS_TextBuf *buffer = as_ctx.text_file->active_buffer;

			// Copy information
			buffer->selection_enabled = selection;
			buffer->selection_start.x = start.x;
			buffer->selection_start.y = start.y;

			if (selection) {
				// Compute the pointer to the selection start
				buffer->selection_start_line = buffer->current_element;
				for (int i = 0; i < abs(CURSOR_Y - start.y); i++) {
					buffer->selection_start_line = (CURSOR_Y > start.y ? buffer->selection_start_line->prev : buffer->selection_start_line->prev);
				}

				// Change selected buffer count
				as_ctx.text_file->selected_buffers += value;
			}

			// Change active buffer indedx
			as_ctx.text_file->active_buffer_idx += value;

			sprintf(as_ctx.editor_scr_message, "BUFFER %s\n", (value == -1 ? "LEFT" : "RIGHT"));
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

	case LOCAL_WINDOW_MOVE: {
		if (value == 1 && as_ctx.text_file->next != NULL) {
			// Move one window to the right
			as_ctx.text_file = as_ctx.text_file->next;
			sprintf(as_ctx.editor_scr_message, "SWITCHED TO %s", as_ctx.text_file->name);
			break;
		}

		if (value == -1 && as_ctx.text_file->prev != NULL) {
			// Move one window to the left
			as_ctx.text_file = as_ctx.text_file->prev;
			sprintf(as_ctx.editor_scr_message, "SWITCHED TO %s", as_ctx.text_file->name);
			break;
		}

		// No movement, let user know
		sprintf(as_ctx.editor_scr_message, "%s", (value == -1 ? "BEGIN" : "END"));

		break;
	}

	case LOCAL_WINDOW_SELECTION: {
		// Enable / disable selection
		as_ctx.text_file->active_buffer->selection_enabled = !as_ctx.text_file->active_buffer->selection_enabled;

		if (as_ctx.text_file->active_buffer->selection_enabled == 0) {
			// Selection was disabled, clear buffers
			for (int i = 0; i < descriptor.column_count; i++) {
				as_ctx.text_file->buffers[i]->selection_enabled = 0;
				as_ctx.text_file->buffers[i]->selection_start_line = NULL;
			}
		}

		// Set the selection start coordinate, reset selected_buffers, store starting line
		as_ctx.text_file->active_buffer->selection_start.x = as_ctx.text_file->active_buffer->cx;
		as_ctx.text_file->active_buffer->selection_start.y = as_ctx.text_file->cy;
		as_ctx.text_file->selected_buffers = 0;
		as_ctx.text_file->active_buffer->selection_start_line = as_ctx.text_file->active_buffer->current_element;

		sprintf(as_ctx.editor_scr_message, "SELECTION\n");

		break;
	}

	case LOCAL_BUFFER_MOVE_LINE: {
		struct AS_TextBuf *buffer = NULL;
		bool moved = 0;

		// TODO: There is probably a cleanr way of doing this
		for (int i = -as_ctx.text_file->selected_buffers; i != 0;) {
			// Move a line down in selected buffers
			buffer = as_ctx.text_file->buffers[as_ctx.text_file->active_buffer_idx + i];

			if (value) {
				moved = buffer_move_ln_up(buffer);
				(buffer->selection_start.y) -= moved;

			} else {
				moved = buffer_move_ln_down(buffer);
				(buffer->selection_start.y) += moved;
			}

			if (as_ctx.text_file->selected_buffers < 0) {
				i--;
			} else {
				i++;
			}
		}

		// Move line down in the first selected buffer
		buffer = as_ctx.text_file->active_buffer;
		if (value) {
			moved = buffer_move_ln_up(buffer);
			(buffer->selection_start.y) -= moved;
		} else {
			moved = buffer_move_ln_down(buffer);
			(buffer->selection_start.y) += moved;
		}
		// TODO END

		// Make sure other buffers are on the same line, that is
		// if they are not selected
		for (int i = 0; i < descriptor.column_count; i++) {
			if (moved && as_ctx.text_file->buffers[i]->selection_enabled == 0 &&
			    as_ctx.text_file->buffers[i]->current_element != NULL && i != as_ctx.text_file->active_buffer_idx) {
				struct AS_LLElement *current = as_ctx.text_file->buffers[i]->current_element;
				as_ctx.text_file->buffers[i]->current_element = (value == 0 ? current->next : current->prev);
			}
		}

		// Update the cursor
		if (moved == 1) {
			CURSOR_Y += (value == 0 ? 1 : -1);
			differential += (value == 0 ? 1 : -1);
		}

		sprintf(as_ctx.editor_scr_message, "LINE MOVE %s\n", (value == 0 ? "DOWN" : "UP"));

		break;
	}

	case LOCAL_FILE_LOAD: {
		switch_to_screen("file_load");

		break;
	}

	case LOCAL_FILE_SAVE: {
		if (value == 1) {
			save_all();
			sprintf(as_ctx.editor_scr_message, "SAVED ALL FILES\n");

			break;
		}

		save_file(as_ctx.text_file);
		sprintf(as_ctx.editor_scr_message, "SAVED FILE\n");

		break;
	}
	}
}

void register_editor_screen() {
	AS_DEBUG_MSG("Registering editor screen\n");

	int i = register_screen("editor", render, update, local);
	as_ctx.screens[i].render_options |= SCR_OPT_ON_UPDATE;
}

struct AS_CfgTok *configure_editor_screen(struct AS_CfgTok *token) {
	return token;
}
