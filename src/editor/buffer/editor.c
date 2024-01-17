/*
*    Assembled - Column based text editor
*    Copyright (C) 2023-2024 awewsomegamer
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
#include <editor/buffer/editor.h>

#include <global.h>
#include <stdio.h>
#include <includes.h>

void load_file_content(struct AS_TextFile *text_file) {
	FILE *file = text_file->file;
	struct AS_ColDesc descriptor = as_ctx.col_descs[as_ctx.col_desc_i];
        int column_count = descriptor.column_count;

	text_file->buffer_count = column_count;

        // Allocate text buffers (columns)
        text_file->buffers = (struct AS_TextBuf **)calloc(column_count, sizeof(struct AS_TextBuf *));
        struct AS_LLElement **currents = (struct AS_LLElement **)calloc(column_count, sizeof(struct AS_LLElement *));

        for (int i = 0; i < column_count; i++) {
                struct AS_TextBuf *buffer = new_buffer(descriptor.column_positions[i], (i + 1 >= column_count) ? -1 :
						       descriptor.column_positions[i + 1]);

                text_file->buffers[i] = buffer;
                currents[i] = buffer->head;
                buffer->current_element = currents[i];
        }

        // Read file
        char *contents;
        size_t size = 0;
        int line_count = 0;

        while (getline(&contents, &size, file) != -1) {
		if (size < 0) {
			break;
		}

                if (contents[strlen(contents) - 1] == '\n') {
                        contents[strlen(contents) - 1] = 0;
                }

                int element = 0;
                int prev_i = 0;

                for (int i = 0; i < strlen(contents) + 1; i++) {
			if (element == column_count - 1) {
				i = strlen(contents) + 1;
				goto read_column;
			}

                        if (contents[i] != descriptor.delimiter && i != strlen(contents)) {
                                continue;
                        }

			read_column:;

                        int element_index = min(element, column_count - 1);

                        // Allocate contents
                        currents[element_index]->contents = (char *)malloc(i - prev_i + 1);
                        memset(currents[element_index]->contents, 0, i - prev_i + 1);

                        // Copy line
                        strncpy(currents[element_index]->contents, contents + prev_i, (i - prev_i));

                        // Allocate new line
                        currents[element_index]->next = (struct AS_LLElement *)malloc(sizeof(struct AS_LLElement));
                        memset(currents[element_index]->next, 0, sizeof(struct AS_LLElement));

                        // Advance
                        currents[element_index]->next->prev = currents[element_index];
                        currents[element_index] = currents[element_index]->next;

                        // Move onto next region of text
                        prev_i = i + 1;

                        // Advance to next column / buffer
                        element++;
                }

                // Fill up rest of the columns
                for (int i = element; i < column_count; i++) {
                        // Allocate contents
                        currents[i]->contents = (char *)malloc(1);
                        *currents[i]->contents = 0;

                        // Allocate new line
                        currents[i]->next = (struct AS_LLElement *)malloc(sizeof(struct AS_LLElement));
                        memset(currents[i]->next, 0, sizeof(struct AS_LLElement));

                        currents[i]->next->prev = currents[i];
                        currents[i] = currents[i]->next;
                }

		if (line_count == text_file->cy) {
			for (int i = 0; i < column_count; i++) {
				text_file->buffers[i]->current_element = currents[i]->prev;
			}
		}

                line_count++;

                // Memory Manage
                free(contents);
                contents = NULL;
        }

	if (line_count <= 1) {
		for (int i = 0; (i < column_count); i++) {
			currents[i]->contents = (char *)malloc(1);
			*(currents[i]->contents) = 0;
			currents[i]->next = NULL;
		}
	} else {
		for (int i = 0; (i < column_count); i++) {
			currents[i]->prev->next = NULL;
			free(currents[i]);
		}
	}

        if (contents != NULL) {
                free(contents);
                contents = NULL;
        }

        text_file->active_buffer = text_file->buffers[0];

        fseek(file, 0, SEEK_SET);

	// Memory Manage
	free(currents);
}

struct AS_TextFile *load_file(char *name) {
        FILE *file = fopen(name, "r+");

        if (file == NULL) {
                AS_DEBUG_MSG("File %s does not exist, creating it\n", name);

                file = fopen(name, "w+");

                // TODO: Replace with a compatible assert
                if (file == NULL) {
                        AS_DEBUG_MSG("Failed to open file %s, exiting\n", name);

                        exit(1);
                }
        }

	// Get the current file.
	struct AS_TextFile *prev = as_ctx.text_file;

        // Allocate text file structure
        as_ctx.text_file = (struct AS_TextFile *)malloc(sizeof(struct AS_TextFile));
	memset(as_ctx.text_file, 0, sizeof(struct AS_TextFile));

	as_ctx.text_file->file = file;
	as_ctx.text_file->name = strdup(name);
	as_ctx.text_file->load_offset = 0;

	// Link the new text file into the list
	if (prev != NULL) {
		as_ctx.text_file->next = prev->next;
		as_ctx.text_file->prev = prev;
		prev->next = as_ctx.text_file;

		if (as_ctx.text_file->next != NULL) {
			as_ctx.text_file->next->prev = as_ctx.text_file;
		}
	}

	if (as_ctx.text_file_head == NULL) {
		// This is the first file opened, keep track of it
		as_ctx.text_file_head = as_ctx.text_file;
	}

	load_file_content(as_ctx.text_file);

        return as_ctx.text_file;
}

void reload_file(struct AS_TextFile *file) {
	if (file == NULL) {
		return;
	}

	AS_DEBUG_MSG("Reloading file %s\n", file->name);

	for (int i = 0; i < file->buffer_count; i++) {
		destroy_buffer(file->buffers[i]);
	}

	load_file_content(file);
}

void reload_all() {
	AS_DEBUG_MSG("Reloading all text files\n");

	struct AS_TextFile *current = as_ctx.text_file_head;
	while (current != NULL) {
		reload_file(current);
		current = current->next;
	}
}

// Save a given file
void save_file(struct AS_TextFile *file) {
	if (file == NULL) {
		return;
	}

	AS_DEBUG_MSG("Saving file %s\n", file->name);

	struct AS_LLElement **currents = (struct AS_LLElement **)calloc(file->buffer_count, sizeof(struct AS_LLElement *));
	
	for (int i = 0; i < file->buffer_count; i++) {
		currents[i] = file->buffers[i]->head;
	}

	size_t file_size = 0;

	fseek(file->file, file->load_offset, SEEK_SET);

	while (currents[0] != NULL) {
		for (int i = 0; i < file->buffer_count; i++) {
			if (currents[i]->contents != NULL) {
				file_size += fputs(currents[i]->contents, file->file);
			}

			if (i < file->buffer_count - 1) {
				fputc(as_ctx.col_descs[as_ctx.col_desc_i].delimiter, file->file);
				file_size++;
			}

			currents[i] = currents[i]->next;
		}
                
                if (currents[0] != NULL) {
		        fputc('\n', file->file);
			file_size++;
                }
	}

	ftruncate(fileno(file->file), file_size);

	// Memory Manage
	free(currents);

	// TODO: Find a better way to do this. This is bad
	fclose(file->file);
	file->file = fopen(file->name, "r+");
}

// Save all files
void save_all() {
	AS_DEBUG_MSG("Saving all text files\n");

	struct AS_TextFile *current = as_ctx.text_file_head;
	while (current != NULL) {
		save_file(current);
		current = current->next;
	}
}

// Destroy a given file
void destroy_file(struct AS_TextFile *file) {
	if (file == NULL) {
		return;
	}

	if (file->prev != NULL) {
		file->prev->next = file->next;
	}

	if (file->next != NULL) {
		file->next->prev = file->prev;
	}

	fclose(file->file);
	
	for (int i = 0; i < as_ctx.col_descs[as_ctx.col_desc_i].column_count; i++) {
		destroy_buffer(file->buffers[i]);
	}

	free(file->name);
	free(file);
}

// Destroy all files
void destroy_all() {
	AS_DEBUG_MSG("Destroying all text files\n");

	struct AS_TextFile *current = as_ctx.text_file_head;
	while (current != NULL) {
		destroy_file(current);
		current = current->next;
	}

	as_ctx.text_file_head = NULL;
	as_ctx.text_file = NULL;
}

struct AS_CfgTok *configure_editor(struct AS_CfgTok *token) {
	// column define:[0, 1, 2, 3, 4, 5, 6]:'c':0
	// column default:0
	AS_EXPECT_TOKEN(AS_CFG_TOKEN_KEY, "Expected keyword")
	
	int value = token->value;
	
	AS_NEXT_TOKEN

	switch (value) {
	case AS_CFG_LOOKUP_DEFINE: {
		AS_EXPECT_TOKEN(AS_CFG_TOKEN_COL, "Expected colon")
		AS_NEXT_TOKEN
		AS_EXPECT_TOKEN(AS_CFG_TOKEN_SQR, "Expected square bracket")
		AS_NEXT_TOKEN

		size_t size = 1;
		int *columns = (int *)malloc(sizeof(int) * size);

		while (token->type != AS_CFG_TOKEN_SQR) {
			if (token->type == AS_CFG_TOKEN_INT) {
				*(columns + size - 1) = token->value;
				columns = (int *)realloc(columns, sizeof(int) * (++size));
			}

			AS_NEXT_TOKEN
		}
		
		// Remove extra on the end
		size--;
		columns = (int *)realloc(columns, sizeof(int) * (size));

		AS_EXPECT_TOKEN(AS_CFG_TOKEN_SQR, "Expected square bracket")
		
		AS_NEXT_TOKEN
		AS_EXPECT_TOKEN(AS_CFG_TOKEN_COL, "Expected colon")
		AS_NEXT_TOKEN
		AS_EXPECT_TOKEN(AS_CFG_TOKEN_INT, "Expected integer")

		int delimiter = token->value;
		
		AS_NEXT_TOKEN
		AS_EXPECT_TOKEN(AS_CFG_TOKEN_COL, "Expected colon")
		AS_NEXT_TOKEN
		AS_EXPECT_TOKEN(AS_CFG_TOKEN_INT, "Expected integer")
		
		int column_descriptor_i = token->value;

		as_ctx.col_descs[column_descriptor_i].column_count = size;
		as_ctx.col_descs[column_descriptor_i].column_positions = columns;
		as_ctx.col_descs[column_descriptor_i].delimiter = delimiter;

		break;
	}

	case AS_CFG_LOOKUP_DEFAULT: {
		AS_EXPECT_TOKEN(AS_CFG_TOKEN_COL, "Expected colon")
		AS_NEXT_TOKEN
		AS_EXPECT_TOKEN(AS_CFG_TOKEN_INT, "Expected integer")

		if (as_ctx.col_descs[token->value].column_positions == NULL) {
			printf("Warning: default column %d is undefined, switching to a defined column\n", token->value);
			AS_DEBUG_MSG("Warning: default column %d is undefined, switching to a defined column\n", token->value);
		}

		for (int i = 0; i < AS_MAX_COLUMNS; i++) {
			if (as_ctx.col_descs[i].column_positions != NULL) {
				as_ctx.col_desc_i = i;
			}
		}

		if (as_ctx.col_desc_i != -1) {
			break;
		}

		AS_DEBUG_MSG("Failed to find any defined column\n");

		break;
	}
	}

        return token;
}
