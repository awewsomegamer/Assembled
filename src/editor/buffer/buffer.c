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

int next_free_buffer = 0;
struct text_buffer *buffers[MAX_BUFFERS];

void free_line_list_element(struct line_list_element *element) {
        free(element->contents);
        free(element);
}

int allocate_buffer() {
        for (int i = 0; i < MAX_BUFFERS; i++) {
                if (buffers[i] == NULL) {
                        return i;
                }
        }

        DEBUG_MSG("Failed to allocate a text buffer\n");

        return -1;
};

struct text_buffer *new_buffer(char *name, FILE *file) {
        struct text_buffer *buffer = (struct text_buffer *)malloc(sizeof(struct text_buffer));
        memset(buffer, 0, sizeof(struct text_buffer));

        buffer->name = name;
        buffer->file = file;
        buffer->cx = 0;
        buffer->cy = 0;
        buffer->load_offset = 0;
        
        buffer->head = (struct line_list_element *)malloc(sizeof(struct line_list_element));
        memset(buffer->head, 0, sizeof(struct line_list_element));

        int i = allocate_buffer();

        if (i == -1) {
                free(buffer->head);
                free(buffer);

                return NULL;
        }

        buffers[i] = buffer;

        return buffer;
}

void save_buffer(struct text_buffer *buffer) {
        // Assume current offset into file is equivalent
        // to the first byte of the first line
        DEBUG_MSG("Saving buffer \"%s\" to offset %d\n", buffer->name, buffer->load_offset);

        fseek(buffer->file, buffer->load_offset, SEEK_SET);

        struct line_list_element *current = buffer->head;
        size_t file_size = 0;

        while (current->next != NULL) {
                fwrite(current->contents, 1, strlen(current->contents), buffer->file);
                fputc('\n', buffer->file);

                file_size += strlen(current->contents);

                current = current->next;
        }

        ftruncate(fileno(buffer->file), file_size + 1);

        DEBUG_MSG("Saved\n");
}

void destroy_buffer(struct text_buffer *buffer) {
        fclose(buffer->file);

        struct line_list_element *current = buffer->head;

        while (current != NULL) {
                struct line_list_element *temp = current->next;
                free(current->contents);
                
                free(current);

                current = temp;
        }
}

// Buffer is the current active buffer

// Insert character c into the current active buffer
void buffer_char_insert(char c) {
        // Get the element at which we need to insert the buffer
        struct line_list_element *element = current_active_text_buffer->head;

        for (int i = 0; i < current_active_text_buffer->cy; i++) {
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
                size_t new_line_size = strlen(element->contents) - current_active_text_buffer->cx;
        
                // Allocate new contents
                char *contents = (char *)malloc(max(new_line_size, 1));
                memset(contents, 0, max(new_line_size, 1));        

                // If the user pressed enter within the line then
                if (new_line_size > 0) {
                        // Copy everything after the cursor to the new line
                        strcpy(contents, element->contents + current_active_text_buffer->cx);

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
                (current_active_text_buffer->cy)++;
                (current_active_text_buffer->cx) = 0; 

                break;
        }

        default: {
                // Default, create new string
                char *new_string = (char *)malloc(strlen(element->contents) + 2); // New character and NULL terminaator
                memset(new_string, 0, strlen(element->contents) + 2);

                // Copy 0 -> cx
                strncpy(new_string, element->contents, current_active_text_buffer->cx);
                // Set space after first portion to new character
                new_string[current_active_text_buffer->cx] = c;
                // Concatenate the last bit of the string
                strcat(new_string, element->contents + current_active_text_buffer->cx);

                // Memory Manage, replace string
                free(element->contents);
                element->contents = new_string;
                
                // Move cursor
                (current_active_text_buffer->cx)++;

                break;
        }
        }

        save_buffer(current_active_text_buffer);
}

void buffer_char_del() {
        if (current_active_text_buffer->cx == 0 && current_active_text_buffer->cy == 0) {
                return;
        }

        struct line_list_element *element = current_active_text_buffer->head;

        // See if the user is aiming to remove the line
        int line_remove = (current_active_text_buffer->cx <= 0);

        for (int i = 0; i < current_active_text_buffer->cy - line_remove; i++) {
                element = element->next;
        }

        DEBUG_MSG("%d\n", (element == NULL));

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

                if (current_active_text_buffer->cy < line) {
                        (current_active_text_buffer->cx) = cx == -1 ? strlen(element->contents) : cx - 1;
                        (current_active_text_buffer->cy)--;
                }

                save_buffer(current_active_text_buffer);

                return;
        }

        char *new_string = strdup(element->contents);
        strncpy(new_string + current_active_text_buffer->cx -1, element->contents + current_active_text_buffer->cx, strlen(element->contents) - current_active_text_buffer->cx);
        new_string[strlen(new_string) - 1] = 0;

        free(element->contents);

        element->contents = new_string;

        (current_active_text_buffer->cx)--;

        save_buffer(current_active_text_buffer);
}
