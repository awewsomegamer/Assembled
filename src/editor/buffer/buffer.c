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
        // Allocate buffer
        struct text_buffer *buffer = (struct text_buffer *)malloc(sizeof(struct text_buffer));
        memset(buffer, 0, sizeof(struct text_buffer));

        // Initialzie values
        buffer->cx = 0;
        buffer->col_start = col_start;
        buffer->col_end = col_end;

        // Give it a starting line list element
        buffer->head = (struct line_list_element *)malloc(sizeof(struct line_list_element));
        memset(buffer->head, 0, sizeof(struct line_list_element));

        return buffer;
}

void destroy_buffer(struct text_buffer *buffer) {
        // Descend the list of line list elements
        // Free each one
        struct line_list_element *current = buffer->head;

        while (current != NULL) {
                struct line_list_element *temp = current->next;
                free(current->contents);
                
                free(current);

                current = temp;
        }

        // Finally free the buffer
        free(buffer);
}

// Buffer is the current active buffer

// Insert character c into the current active buffer
void buffer_char_insert(char c) {
        // Get the element at which we need to insert the buffer
        struct text_buffer *active_text_buffer = active_text_file->active_buffer;
        struct line_list_element *element = active_text_buffer->current_element;

        // Check for special cases
        switch (c) {
        case '\n': {
                for (int i = 0; i < column_descriptors[current_column_descriptor].column_count; i++) {
                        if (active_text_file->buffers[i] != active_text_buffer) {
                                struct line_list_element *tmp = active_text_file->buffers[i]->current_element;

                                struct line_list_element *new_element = (struct line_list_element *)malloc(sizeof(struct line_list_element));

                                new_element->contents = (char *)malloc(1);
                                *(new_element->contents) = 0;

                                new_element->next = tmp->next;
                                new_element->prev = tmp;
                                
                                if (tmp->next != NULL) {
                                        tmp->next->prev = new_element;
                                }

                                tmp->next = new_element;
                        }
                }

                // Create new element
                struct line_list_element *next_element = (struct line_list_element *)malloc(sizeof(struct line_list_element));

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

                // Increment cy and reset cx
                (active_text_file->cy)++;
                (active_text_buffer->cx) = 0;

                // Manage
                active_text_buffer->current_element = next_element;

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

        // Temporary save
        save_file(active_text_file);
}

void buffer_char_del() {
        // Get the element at which we need to insert the buffer
        struct text_buffer *active_text_buffer = active_text_file->active_buffer;

        // Has the cursor reached (0, 0), if so we can't delete further
        if (active_text_buffer->cx == 0 && active_text_file->cy == 0) {
                return;
        }

        struct line_list_element *element = active_text_buffer->current_element;

        // Remove a line
        if (active_text_buffer->cx <= 0) {
                struct line_list_element *line_over = element->next;
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
                (active_text_buffer->cx) = cx == -1 ? strlen(element->contents) : cx - 1;
                (active_text_file->cy)--;

                // Manage
                active_text_buffer->current_element = element;

                // Temporary save
                save_file(active_text_file);

                return;
        }

        // Remove a character
        // Allocate a new string
        char *new_string = strdup(element->contents);
        // Copy latter half over the first -1 character
        strncpy(new_string + active_text_buffer->cx -1, element->contents + active_text_buffer->cx, strlen(element->contents) - active_text_buffer->cx);
        new_string[strlen(new_string) - 1] = 0;

        // Memory Manage
        free(element->contents);

        element->contents = new_string;

        (active_text_buffer->cx)--;

        // Temporary save
        save_file(active_text_file);
}
