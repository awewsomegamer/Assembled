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
#include <editor/config.h>
#include <global.h>
#include <editor/buffer/editor.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct text_file *text_files[MAX_TEXT_FILES] = { 0 };
struct column_descriptor column_descriptors[MAX_COLUMNS] = { 0 };

int free_text_file = 0;

struct text_file *active_text_file = NULL;

int data[] = {0, 40};
struct text_file *load_file(char *name) {
        FILE *file = fopen(name, "r+");

        if (file == NULL) {
                DEBUG_MSG("File %s does not exist, creating it\n", name);

                file = fopen(name, "w+");

                // TODO: Replace with a compatible assert
                if (file == NULL) {
                        DEBUG_MSG("Failed to open file %s, exiting\n");
                
                        exit(1);
                }
        }

        // Find a free space
        int x = -1;
        for (int i = 0; i < MAX_TEXT_FILES; i++) {
                if (text_files[i] == NULL) {
                        x = i;
                        break;
                }
        }

        // Check if there is an allocated position
        if (x == -1) {
                DEBUG_MSG("Failed to allocate a text buffer");
		
                return NULL;
        }
        
        // Allocate text file structure
        active_text_file = (struct text_file *)malloc(sizeof(struct text_file));
	memset(active_text_file, 0, sizeof(struct text_file));
	
	active_text_file->file = file;
	active_text_file->name = name;
	active_text_file->load_offset = 0;

        text_files[x] = active_text_file;

        // TEMPORARY
        column_descriptors[0].column_count = 2;
        column_descriptors[0].column_positions = data;
        column_descriptors[0].delimiter = '\t';
        // /TEMPORARY

        // Allocate text buffers (columns)
        int column_count = column_descriptors[0].column_count;
        active_text_file->buffers = (struct text_buffer **)calloc(column_count, sizeof(struct text_buffer *));
        struct line_list_element **currents = (struct line_list_element **)calloc(column_count, sizeof(struct line_list_element *));

        int prev_column = 0;
        for (int i = 0; i < column_count; i++) {
                struct text_buffer *buffer = new_buffer(prev_column, column_descriptors[0].column_positions[i]);
                prev_column = column_descriptors[0].column_positions[i];
                
                active_text_file->buffers[i] = buffer;
                currents[i] = buffer->head;
        }

        // Read file
        char *contents;
        size_t size = 0;
        int line_count = 1;

        while (getline(&contents, &size, file) != -1) {
                if (contents[strlen(contents) - 1] == '\n') {
                        contents[strlen(contents) - 1] = 0;
                }
                
                int buffer = 0;
                int prev_i = 0;

                // TODO: If our current delimiter > column count
                //       then continue, when we reach the end of
                //       the line, insert the string.
                for (int i = 0; i < strlen(contents) + 1; i++) {
                        if (contents[i] != column_descriptors[0].delimiter && i != strlen(contents)) {
                                continue;
                        }

                        int buffer_index = min(buffer, column_count - 1);

                        // Clear memory
                        memset(currents[buffer_index], 0, sizeof(struct line_list_element));

                        // Allocate contents
                        currents[buffer_index]->contents = (char *)malloc(i - prev_i + 1);
                        memset(currents[buffer_index]->contents, 0, i - prev_i + 1);

                        // Copy line and set line count
                        strncpy(currents[buffer_index]->contents, contents + prev_i, (i - prev_i));
                        currents[buffer_index]->line = line_count;

                        // Allocate new line
                        currents[buffer_index]->next = (struct line_list_element *)malloc(sizeof(struct line_list_element));
                        memset(currents[buffer_index]->next, 0, sizeof(struct line_list_element));

                        // Advance
                        currents[buffer_index] = currents[buffer_index]->next;
                        
                        // Move onto next region of text
                        prev_i = i + 1;

                        // Advance to next column / buffer
                        buffer++;
                }

                // Fill up rest of the columns
                for (int i = buffer; i < column_count; i++) {
                        // Allocate contents
                        currents[i]->contents = (char *)malloc(1);
                        *currents[i]->contents = 0;

                        // Copy line and set line count
                        currents[i]->line = line_count;

                        // Allocate new line
                        currents[i]->next = (struct line_list_element *)malloc(sizeof(struct line_list_element));
                        memset(currents[i]->next, 0, sizeof(struct line_list_element));

                        currents[i] = currents[i]->next;

                }

                line_count++;

                // Memory Manage
                free(contents);
                contents = NULL;
        }
        
        for (int i = 0; i < column_count; i++) {
                currents[i]->line = line_count;
                currents[i]->contents = (char *)malloc(1);
                *(currents[i]->contents) = 0;
                currents[i]->next = NULL;
        }

        if (contents != NULL) {
                free(contents);
                contents = NULL;
        }

        active_text_file->active_buffer = active_text_file->buffers[0];

        fseek(file, 0, SEEK_SET);

	// Memory Manage
	free(currents);

        return active_text_file;
}

void save_file(struct text_file *file) {
	int column_count = column_descriptors[0].column_count;

	struct line_list_element **currents = (struct line_list_element **)calloc(column_count, sizeof(struct line_list_element *));
	
	for (int i = 0; i < column_count; i++) {
		currents[i] = file->buffers[i]->head;
	}

	size_t file_size = 0;

	fseek(file->file, file->load_offset, SEEK_SET);

	while (currents[0]->next != NULL) {
		for (int i = 0; i < column_count; i++) {
			fputs(currents[i]->contents, file->file);

			if (i < column_count - 1) {
				fputc(column_descriptors[0].delimiter, file->file);
			}

			currents[i] = currents[i]->next;
		}

		fputc('\n', file->file);
	}

	ftruncate(fileno(file->file), file_size);

	// Memory Manage
	free(currents);
}

void edit_file() {

}

struct cfg_token *init_editor(struct cfg_token *token) {
        return token;
}