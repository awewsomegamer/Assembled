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
void buffer_char_insert(char c) {
        struct line_list_element *element = current_active_text_buffer->head;

        for (int i = 0; i < current_active_text_buffer->cy; i++) {
                element = element->next;
        }

        switch (c) {
        case '\n': {
                struct line_list_element *next_element = (struct line_list_element *)malloc(sizeof(struct line_list_element));
                next_element->next = element->next;

                char *contents = (char *)malloc(1);
                *contents = 0;

                next_element->contents = contents;
                
                struct line_list_element *current = next_element;
                int line = element->line + 1;

                while (current != NULL) {
                        current->line = line++;
                        
                        current = current->next;
                }

                element->next = next_element;

                (current_active_text_buffer->cy)++;
                (current_active_text_buffer->cx) = 0; 

                break;
        }

        default: {
                if (c < 32) {
                        break;
                }

                char *new_string = (char *)malloc(strlen(element->contents) + 2); // New character and NULL terminaator
                memset(new_string, 0, strlen(element->contents) + 2);

                strncpy(new_string, element->contents, current_active_text_buffer->cx);
                new_string[current_active_text_buffer->cx] = c;
                strcat(new_string, element->contents + current_active_text_buffer->cx);

                free(element->contents);
                element->contents = new_string;

                (current_active_text_buffer->cx)++;

                break;
        }
        }

        save_buffer(current_active_text_buffer);
}

void buffer_char_del() {
        struct line_list_element *element = current_active_text_buffer->head;

        int line_remove = (current_active_text_buffer->cx <= 0);

        for (int i = 0; i < current_active_text_buffer->cy - line_remove; i++) {
                element = element->next;
        }

        if (line_remove) {
                int line = element->line + 1;

                struct line_list_element *current = element->next->next;
                
                free_line_list_element(element->next);
                element->next = current;

                while (current != NULL) {
                        current->line = line++;

                        current = current->next;
                }

                if (current_active_text_buffer->cy < line) {
                        (current_active_text_buffer->cx) = strlen(element->contents);
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
