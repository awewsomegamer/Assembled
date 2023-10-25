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
#include <editor/buffer/buffer.h>
#include <global.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int next_free_buffer = 0;
struct text_buffer *buffers[MAX_BUFFERS];

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

        buffer->name = name;
        buffer->file = file;
        buffer->cx = 0;
        buffer->cy = 0;
        buffer->load_offset = 0;
        
        buffer->head = (struct line_list_element *)malloc(sizeof(struct line_list_element));

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
        
        while (current->next != NULL) {
                fwrite(current->contents, 1, strlen(current->contents), buffer->file);

                current = current->next;
        }

        DEBUG_MSG("Saved\n");
}

void destroy_buffer(struct text_buffer *buffer) {
        fclose(buffer->file);

        struct line_list_element *current = buffer->head;

        while (current->next != NULL) {
                struct line_list_element *temp = current->next;
                free(current->contents);
                
                free(current);

                current = temp;
        }
}

// Buffer is the current active buffer
void insert_into_buffer(char c) {
        struct line_list_element *element = current_active_text_buffer->head;

        for (int i = 0; i < current_active_text_buffer->cy; i++) {
                element = element->next;
        }

        char *new_string = (char *)malloc(strlen(element->contents) + 2); // New character and NULL terminaator
        memset(new_string, 0, strlen(element->contents) + 2);

        memcpy(new_string, element->contents, current_active_text_buffer->cx);
        new_string[current_active_text_buffer->cx] = c;
        strcat(new_string, element->contents + current_active_text_buffer->cx);

        free(element->contents);
        element->contents = new_string;

        (current_active_text_buffer->cx)++;

        save_buffer(current_active_text_buffer);
}
