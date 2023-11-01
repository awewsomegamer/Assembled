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

#include "editor/buffer/editor.h"
#include <ctype.h>
#include <editor/buffer/buffer.h>
#include <global.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void free_line_list_element(struct line_list_element *element) {
        free(element->contents);
        free(element);
}

struct text_buffer *new_buffer(int col_start, int col_end) {
        struct text_buffer *buffer = (struct text_buffer *)malloc(sizeof(struct text_buffer));
        memset(buffer, 0, sizeof(struct text_buffer));

        buffer->cx = 0;
        buffer->col_start = col_start;
        buffer->col_end = col_end;

        buffer->head = (struct line_list_element *)malloc(sizeof(struct line_list_element));
        memset(buffer->head, 0, sizeof(struct line_list_element));

        return buffer;
}

void destroy_buffer(struct text_buffer *buffer) {
        struct line_list_element *current = buffer->head;

        while (current != NULL) {
                struct line_list_element *temp = current->next;
                free(current->contents);
                
                free(current);

                current = temp;
        }

        free(buffer);
}

// Buffer is the current active buffer
// No longer a single buffer, there are multiple.
// New lines and line deletions have to effect all buffers
// ^ Ties into ERROR left buffer_char_del();

// Insert character c into the current active buffer
void buffer_char_insert(char c) {
        // Get the element at which we need to insert the buffer
        struct text_buffer *active_text_buffer = active_text_file->active_buffer;

        struct line_list_element *element = active_text_buffer->head;

        for (int i = 0; i < active_text_file->cy; i++) {
                element = element->next;
        }

        // Check for special cases
        switch (c) {
        case '\n': {
                // Create new line
                // Create new element and start to insert it after the element user is on
                struct line_list_element *next_element = (struct line_list_element *)malloc(sizeof(struct line_list_element));
                next_element->next = element->next;

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
                        char *tmp = (char *)malloc(post_copy_size);

                        // Into the newly allocated buffer, copy all the characters left
                        strncpy(tmp, element->contents, post_copy_size);
                        // Memory Manage
                        free(element->contents);

                        // Set the contents to be to up to date
                        element->contents = tmp;
                }

                // Set contents
                next_element->contents = contents;
                
                // Update line integers from the new element to the last
                struct line_list_element *current = next_element;
                int line = element->line + 1;

                while (current != NULL) {
                        current->line = line++;
                        
                        current = current->next;
                }

                // Complete insertion
                element->next = next_element;

                // Increment cy and reset cx
                (active_text_file->cy)++;
                (active_text_buffer->cx) = 0; 

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
        save_file(active_text_file);
}

void buffer_char_del() {
        struct text_buffer *active_text_buffer = active_text_file->active_buffer;

        if (active_text_buffer->cx == 0 && active_text_file->cy == 0) {
                return;
        }

        struct line_list_element *element = active_text_buffer->head;

        // See if the user is aiming to remove the line
        int line_remove = (active_text_buffer->cx <= 0);

        for (int i = 0; i < active_text_file->cy - line_remove; i++) {
                element = element->next;
        }

        DEBUG_MSG("%d\n", (element == NULL));

	// ERROR: Lines aren't being removed
        if (line_remove) {
                int line = element->line + 1;

                struct line_list_element *line_over = element->next->next;
                
                int cx = -1;

                // If the line has characters on it, add them to the previous line
                if (strlen(element->next->contents) > 0) {
                        cx = strlen(element->contents);

                        size_t size = cx + strlen(element->next->contents) + 1;
                        element->contents = (char *)realloc(element->contents, size);
                        strcat(element->contents, element->next->contents);
                }

                free_line_list_element(element->next);
                
                element->next = line_over;

                while (line_over != NULL) {
                        line_over->line = line++;

                        line_over = line_over->next;
                }

                if (active_text_file->cy < line) {
                        (active_text_buffer->cx) = cx == -1 ? strlen(element->contents) : cx - 1;
                        (active_text_file->cy)--;
                }

                save_file(active_text_file);

                return;
        }

        char *new_string = strdup(element->contents);
        strncpy(new_string + active_text_buffer->cx -1, element->contents + active_text_buffer->cx, strlen(element->contents) - active_text_buffer->cx);
        new_string[strlen(new_string) - 1] = 0;

        free(element->contents);

        element->contents = new_string;

        (active_text_buffer->cx)--;

        save_file(active_text_file);
}
