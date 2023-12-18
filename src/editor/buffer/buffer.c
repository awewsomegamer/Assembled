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
#include <editor/buffer/buffer.h>

#include <interface/interface.h>

#include <global.h>
#include <includes.h>

void free_line_list_element(struct AS_LLElement *element) {
	free(element->contents);
	free(element);
}

struct AS_TextBuf *new_buffer(int col_start, int col_end) {
	// Allocate buffer
	struct AS_TextBuf *buffer = (struct AS_TextBuf *)malloc(sizeof(struct AS_TextBuf));
	memset(buffer, 0, sizeof(struct AS_TextBuf));

	// Initialzie values
	buffer->cx = 0;
	buffer->col_start = col_start;
	buffer->col_end = col_end;

	// Give it a starting line list element
	buffer->head = (struct AS_LLElement *)malloc(sizeof(struct AS_LLElement));
	buffer->virtual_head = buffer->head;
	memset(buffer->head, 0, sizeof(struct AS_LLElement));

	return buffer;
}

void destroy_buffer(struct AS_TextBuf *buffer) {
	// Descend the list of line list elements
	// Free each one
	struct AS_LLElement *current = buffer->head;

	while (current != NULL) {
		struct AS_LLElement *temp = current->next;
		free(current->contents);

		free(current);

		current = temp;
	}

	// Finally free the buffer
	free(buffer);
}

// Buffer is the current active buffer

// Insert character c into the current active buffer

// ERROR: New lines sometimes result in leaks 
//	  (random characters on a string due
//	   a lack of a zero terminator)
void buffer_char_insert(char c) {
	// Get the element at which we need to insert the buffer
	struct AS_TextBuf *active_text_buffer = as_ctx.text_file->active_buffer;
	struct AS_LLElement *element = active_text_buffer->current_element;

	// Check for special cases
	switch (c) {
	case '\n': {
		// Iterate through all buffers excluding the active one
		for (int i = 0; i < as_ctx.col_descs[as_ctx.col_desc_i].column_count; i++) {
			if (as_ctx.text_file->buffers[i] == active_text_buffer) {
				continue;
			}

			// Create a new line and its element
			struct AS_LLElement *tmp = as_ctx.text_file->buffers[i]->current_element;
			struct AS_LLElement *new_element = (struct AS_LLElement *)malloc(sizeof(struct AS_LLElement));

			// Allocate contents
			new_element->contents = (char *)malloc(1);
			*(new_element->contents) = 0;
			
			// Check if the new line will be at the beginning 
			// of a line
			if (active_text_buffer->cx > 0) {
				// If it won't, link it up normally
				// by placing the new line after the
				// current line
				new_element->next = tmp->next;
				new_element->prev = tmp;
				
				if (tmp->next != NULL) {
					tmp->next->prev = new_element;
				}

				tmp->next = new_element;
			} else {
				// If it will, place the new line before
				// the current line
				new_element->next = tmp;
				new_element->prev = tmp->prev;

				if (tmp->prev != NULL) {
					tmp->prev->next = new_element;
				}

				tmp->prev = new_element;

				if (tmp == as_ctx.text_file->buffers[i]->head) {
					as_ctx.text_file->buffers[i]->head = new_element;
				}
			}

			// Manage the current element
			as_ctx.text_file->buffers[i]->current_element = active_text_buffer->cx > 0 ? new_element : tmp;  
		}

		// Create new element
		struct AS_LLElement *next_element = (struct AS_LLElement *)malloc(sizeof(struct AS_LLElement));

		// Insert it into list
		next_element->next = element->next;
		next_element->prev = element;

		if (element->next != NULL) {
			element->next->prev = next_element;
		}

		element->next = next_element;

		// See if user pressed enter within the line
		size_t new_line_size = strlen(element->contents) - active_text_buffer->cx;

		// Allocate new contents
		char *contents = (char *)malloc(max(new_line_size, 1));
		memset(contents, 0, max(new_line_size, 1));

		// If the user pressed enter within the line then
		if (new_line_size > 0) {
			// Copy everything after the cursor to the new line
			strcpy(contents, element->contents + active_text_buffer->cx);

			// Calculate the number of characters left in this line
			size_t post_copy_size = strlen(element->contents) - new_line_size;
			// Adjust allocation
			element->contents = (char *)realloc(element->contents, post_copy_size + 1);
			// Null terminate
			*(element->contents + post_copy_size) = 0;
		}

		// Set contents
		next_element->contents = contents;

		// Increment cy and reset cx
		(as_ctx.text_file->cy)++;
		(active_text_buffer->cx) = 0;

		// Manage
		active_text_buffer->current_element = next_element;

		AS_DEBUG_MSG("Calling LOCAL_LINE_INSERT\n");
		as_ctx.screen->local(LOCAL_LINE_INSERT, 0);

		break;
	}

	default: {
		// Default, create new string
		char *new_string = (char *)malloc(strlen(element->contents) + 2); // New character and NULL terminaator
		memset(new_string, 0, strlen(element->contents) + 2);

		// Copy 0 -> cx
		strncpy(new_string, element->contents, active_text_buffer->cx);
		// Set space after first portion to new character
		new_string[active_text_buffer->cx] = c;
		// Concatenate the last bit of the string
		strcat(new_string, element->contents + active_text_buffer->cx);

		// Memory Manage, replace string
		free(element->contents);
		element->contents = new_string;

		// Move cursor
		(active_text_buffer->cx)++;

		break;
	}
	}
}

void buffer_char_del() {
	// Get the element at which we need to insert the buffer
	struct AS_TextBuf *active_text_buffer = as_ctx.text_file->active_buffer;

	// Has the cursor reached (0, 0), if so we can't delete further
	if (active_text_buffer->cx == 0 && as_ctx.text_file->cy == 0) {
		return;
	}

	struct AS_LLElement *element = active_text_buffer->current_element;

	// Remove a line
	if (active_text_buffer->cx <= 0) {
		// Iterate through all buffers
		for (int i = 0; i < as_ctx.col_descs[as_ctx.col_desc_i].column_count; i++) {
			// Get the current element
			element = as_ctx.text_file->buffers[i]->current_element;

			// Get the address of the next door neighbour
			struct AS_LLElement *line_over = element->next;
			// Move one house down
			element = element->prev;
			
			int cx = -1;

			// If the line has characters on it, add them to the previous line
			if (strlen(element->next->contents) > 0) {
				cx = strlen(element->contents);

				size_t size = cx + strlen(element->next->contents) + 1;
				element->contents = (char *)realloc(element->contents, size);
				strcat(element->contents, element->next->contents);
			}

			// Memory Manage
			free_line_list_element(element->next);
			
			// Update links
			element->next = line_over;

			if (line_over != NULL) {
				line_over->prev = element;
			}

			// Update cursor
			(as_ctx.text_file->buffers[i]->cx) = cx == -1 ? strlen(element->contents) : cx - 1;

			if (as_ctx.text_file->buffers[i]->cx < 0) {
				as_ctx.text_file->buffers[i]->cx = 0;
			}

			// Manage
			as_ctx.text_file->buffers[i]->current_element = element;
		}

		(as_ctx.text_file->cy)--;

		as_ctx.screen->local(LOCAL_LINE_DELETION, 0);

		return;
	}

	// Remove a character
	// Allocate a new string
	char *new_string = strdup(element->contents);
	// Copy latter half over the first -1 character
	strncpy(new_string + active_text_buffer->cx - 1, element->contents + active_text_buffer->cx, strlen(element->contents) - active_text_buffer->cx);
	new_string[strlen(new_string) - 1] = 0;

	// Memory Manage
	free(element->contents);

	element->contents = new_string;

	(active_text_buffer->cx)--;
}

// ERROR: The nature of the multiline movement code is
//        questionable as it does not work across multiple
//        buffers
// Move the current line, or selected lines, down in the given buffer
// Returns 1 on success, returns 0 on failure ("%d lines moved", return_code)
int buffer_move_ln_up(struct AS_TextBuf *active_buffer) {
	if (active_buffer == NULL) {
		return 0;
	}

	struct AS_LLElement *current = active_buffer->current_element;
	struct AS_LLElement *next = current->next;
	struct AS_LLElement *prev = current->prev;

	if (active_buffer->selection_enabled && as_ctx.text_file->cy != active_buffer->selection_start.y) {
		// There is a selection, move selection
		// Get the head of the selection
		struct AS_LLElement *head = active_buffer->selection_start_line;
		// Prev is no longer current's prev
		prev = head->prev;

		if (as_ctx.text_file->cy < active_buffer->selection_start.y) {
			// Above pointers are the wrong way around, correct them
			head = current;
			prev = current->prev;
			current = active_buffer->selection_start_line;
			next = current->next;
		}

		if (prev == NULL) {
			// No where to move the line
			return 0;
		}

		// Head takes the position of prev pt. 1
		head->prev = prev->prev;

		if (prev->prev == NULL) {
			// Prev was buffer's head, set head to buffer's head
			active_buffer->head = head;
		} else {
			// Tell prev->prev that head is now prev
			prev->prev->next = head;
		}

		// Head takes the position of prev pt. 2
		prev->prev = current;
		current->next = prev;
		prev->next = next;

		if (next != NULL) {
			// Selection wasn't at the bottom of the file
			// point next->prev to prev
			next->prev = prev;
		}

		return 1;
	}

	// Selection is not enabled, move a single line
	if (prev == NULL) {
		// No where to move the line
		return 0;
	}

	// Current takes the place of prev
	current->next = prev;
	current->prev = prev->prev;

	// Check if prev was the head
	if (current->prev == NULL) {
		// It was, current is now the head
		active_buffer->head = current;
	} else {
		// Point prev->prev's next at current
		prev->prev->next = current;
	}

	if (next != NULL) {
		// Current was not at the bottom
		// of the file, update next's prev
		next->prev = prev;
	}

	// Change prev's links to take current's spot
	prev->next = next;
	prev->prev = current;

	return 1;
}

// Move the current line, or selected lines, down in the given buffer
int buffer_move_ln_down(struct AS_TextBuf *active_buffer) {
	if (active_buffer == NULL) {
		return 0;
	}

	struct AS_LLElement *current = active_buffer->current_element;
	struct AS_LLElement *next = current->next;
	struct AS_LLElement *prev = current->prev;

	if (active_buffer->selection_enabled && as_ctx.text_file->cy != active_buffer->selection_start.y) {
		// There is a selection, move selection
		struct AS_LLElement *head = active_buffer->selection_start_line;
		// Prev is no longer current's prev
		prev = active_buffer->selection_start_line->prev;

		if (as_ctx.text_file->cy < active_buffer->selection_start.y) {
			// Above pointers are the wrong way around, correct them
			head = current;
			prev = current->prev;
			current = active_buffer->selection_start_line;
			next = current->next;
		}

		if (next == NULL) {
			// No where to move the line
			return 0;
		}

		if (next->next != NULL) {
			// The selection is not at the end of the file
			// tell next->next element where to find its prev
			next->next->prev = current;
		}

		if (prev != NULL) {
			// Selection is not at the top of the file
			// tell prev where to find its next element
			prev->next = next;
		} else {
			// Selection was at the top of the file,
			// update head
			active_buffer->head = next;
		}

		// Head takes the place of next
		current->next = next->next;
		next->next = head;
		next->prev = prev;
		head->prev = next;

		return 1;
	}

	// Selection is not enabled, move a single line
	if (next == NULL) {
		// No where to move the line
		return 0;
	}

	if (next->next != NULL) {
		// Line is not at the bottom of the file
		next->next->prev = current;
	}

	if (prev != NULL) {
		// Line is not at the top of the file
		// update prev's next element
		prev->next = next;
	} else {
		// Line was at the top of the file
		// update head
		active_buffer->head = next;
	}

	// Current takes place of next
	current->next = next->next;
	next->next = current;
	next->prev = current->prev;
	current->prev = next;

	return 1;
}
