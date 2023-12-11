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

#define CURSOR_X (as_ctx.text_file->active_buffer->cx)
#define CURSOR_Y (as_ctx.text_file->cy)

static int line_length = 0;

static int differential = 0;
static int offset = 0;

char AS_EditorScrMessage[1024];

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
	mvprintw(context->max_y - 1, 0, "EDITING (%d, %d) %s", CURSOR_Y + 1, CURSOR_X + 1, AS_EditorScrMessage);

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
	case LOCAL_ARROW_UP: {
		bool moved = 0;

		// Update all current pointers one up
		for (int i = 0; i < descriptor.column_count; i++) {
			struct AS_LLElement *current = as_ctx.text_file->buffers[i]->current_element;

			if (current->prev != NULL) {
				as_ctx.text_file->buffers[i]->current_element = current->prev;
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
			struct AS_LLElement *current = as_ctx.text_file->buffers[i]->current_element;

			if (current->next != NULL) {
				as_ctx.text_file->buffers[i]->current_element = current->next;
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

	// TODO: Buffers which have already been selected
	//       can be deselected.
	case LOCAL_BUFFER_MOVE: {
		int i = as_ctx.text_file->active_buffer_idx;

		if (i + value >= 0 && i + value < descriptor.column_count) {
			struct AS_Bound start = as_ctx.text_file->active_buffer->selection_start;
			uint8_t selection = as_ctx.text_file->active_buffer->selection_enabled;

			as_ctx.text_file->active_buffer = as_ctx.text_file->buffers[i + value];
			as_ctx.text_file->active_buffer->selection_enabled = selection;
			as_ctx.text_file->active_buffer->selection_start.x = start.x;
			as_ctx.text_file->active_buffer->selection_start.y = start.y;

			as_ctx.text_file->selected_buffers += value;
			as_ctx.text_file->active_buffer_idx += value;
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
		int i = min(as_ctx.text_file_i - 1, 0);

		for (; i >= 0; i--) {
			if (as_ctx.text_files[i] != NULL) {
				break;
			}
		}

		if (as_ctx.text_files[i] != NULL) {
			as_ctx.text_file = as_ctx.text_files[i];
			as_ctx.text_file_i = i;

			sprintf(AS_EditorScrMessage, "SWITCHED TO %s", as_ctx.text_file->name);
		} else {
			sprintf(AS_EditorScrMessage, "BEGIN");
		}

		break;
	}

	case LOCAL_WINDOW_RIGHT: {
		int i = min(as_ctx.text_file_i + 1, MAX_TEXT_FILES - 1);

		for (; i < MAX_TEXT_FILES; i++) {
			if (as_ctx.text_files[i] != NULL) {
				break;
			}
		}

		if (as_ctx.text_files[i] != NULL && i < MAX_TEXT_FILES) {
			as_ctx.text_file = as_ctx.text_files[i];
			as_ctx.text_file_i = i;

			sprintf(AS_EditorScrMessage, "SWITCHED TO %s %d", as_ctx.text_file->name, i);
		} else {
			sprintf(AS_EditorScrMessage, "END");
		}
		
		break;
	}

	case LOCAL_WINDOW_SELECTION: {
		as_ctx.text_file->active_buffer->selection_enabled = !as_ctx.text_file->active_buffer->selection_enabled;

		if (as_ctx.text_file->active_buffer->selection_enabled == 0) {
			for (int i = 0; i < descriptor.column_count; i++) {
				as_ctx.text_file->buffers[i]->selection_enabled = 0;
			}
		}

		as_ctx.text_file->active_buffer->selection_start.x = as_ctx.text_file->active_buffer->cx;
		as_ctx.text_file->active_buffer->selection_start.y = as_ctx.text_file->cy;
		as_ctx.text_file->selected_buffers = 0;
		as_ctx.text_file->active_buffer->selection_start_line = as_ctx.text_file->active_buffer->current_element;

		break;
	}

	case LOCAL_BUFFER_MOVE_LINE: {
		int lines_moved = 0;

		for (int i = 0; i < abs(as_ctx.text_file->selected_buffers) + 1; i++) {
			struct AS_TextBuf *buffer = as_ctx.text_file->active_buffer;

			if (as_ctx.text_file->selected_buffers < 0) {
				buffer = as_ctx.text_file->buffers[as_ctx.text_file->active_buffer_idx - i];
			} else if (as_ctx.text_file->selected_buffers == 0) {
				buffer = as_ctx.text_file->active_buffer;
			} else {
				buffer = as_ctx.text_file->buffers[as_ctx.text_file->active_buffer_idx + i];
			}

			if (value == 1) {
				lines_moved = buffer_move_ln_up(buffer);
				(buffer->selection_start.y) -= lines_moved;

				continue;
			}

			lines_moved = buffer_move_ln_down(buffer);
			(buffer->selection_start.y) += lines_moved;
		}

		if (lines_moved == 0) {
			break;
		}

		int a = min(as_ctx.text_file->active_buffer_idx, as_ctx.text_file->active_buffer_idx + as_ctx.text_file->selected_buffers);
		int b = max(as_ctx.text_file->active_buffer_idx, as_ctx.text_file->active_buffer_idx + as_ctx.text_file->selected_buffers);
		bool moved = 0;

		for (int i = 0; i < descriptor.column_count; i++) {
			if (a < i && i < b || i == as_ctx.text_file->active_buffer_idx) {
				continue;
			}

			struct AS_LLElement *element = as_ctx.text_file->buffers[i]->current_element;
			struct AS_LLElement *new = (value == 0 ? element->next : element->prev);

			if (new != NULL) {
				as_ctx.text_file->buffers[i]->current_element = new;
				moved = 1;
			}
		}

		if (moved == 1) {
			CURSOR_Y += (value == 0 ? 1 : -1);
			differential += (value == 0 ? 1 : -1);
		}

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
