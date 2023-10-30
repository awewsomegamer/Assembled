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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int editor_line = 0;
int editor_column = 0;

struct text_file *text_files[32];
int free_text_file = 0;

struct text_file *active_text_file = NULL;

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

        int x = -1;
        for (int i = 0; i < 32; i++) {
                if (text_files[i] == NULL) {
                        x = i;
                        break;
                }
        }

        if (x == -1) {
                DEBUG_MSG("Failed to allocate a text buffer");
                return NULL;
        }

        active_text_file = (struct text_file *)malloc(sizeof(struct text_file));
        text_files[x] = active_text_file;



        char *contents;
        size_t size = 0;
        int line_count = 1;
        struct line_list_element *cur_element = active_text_file->head;

        while (getline(&contents, &size, file) != -1) {
                if (contents[strlen(contents) - 1] == '\n') {
                        contents[strlen(contents) - 1] = 0;
                }
                
                memset(cur_element, 0, sizeof(struct line_list_element));

                cur_element->contents = strdup(contents);

                cur_element->line = line_count++;
                cur_element->next = (struct line_list_element *)malloc(sizeof(struct line_list_element));
                memset(cur_element->next, 0, sizeof(struct line_list_element));
                
                free(contents);
                contents = NULL;
                
                cur_element = cur_element->next;
        }

        cur_element->line = line_count;
        cur_element->contents = (char *)malloc(1);
        *(cur_element->contents) = 0;
        cur_element->next = NULL;

        if (contents != NULL) {
                free(contents);
                contents = NULL;
        }

        if (active_text_file->head->contents == NULL) {
                active_text_file->head->contents = (char *)malloc(1);
        }

        fseek(file, 0, SEEK_SET);

        return active_text_file;
}

void edit_file() {

}

struct cfg_token *init_editor(struct cfg_token *token) {
        return token;
}