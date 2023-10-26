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
struct text_buffer *current_active_text_buffer = NULL;

// TODO: Unable to load empty files
//       and edit imemdiately. Make
//       this possible.
struct text_buffer *load_file(char *name) {
        FILE *file = fopen(name, "r+");

        current_active_text_buffer = new_buffer(name, file);

        if (file == NULL) {
                DEBUG_MSG("File %s does not exist, creating a new buffer\n", name);

                return current_active_text_buffer;
        }
        
        char *contents;
        size_t size = 0;
        int line_count = 1;
        struct line_list_element *cur_element = current_active_text_buffer->head;

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

        if (contents != NULL) {
                free(contents);
                contents = NULL;
        }

        if (current_active_text_buffer->head->contents == NULL) {
                current_active_text_buffer->head->contents = (char *)malloc(1);
        }

        fseek(file, 0, SEEK_SET);

        return current_active_text_buffer;
}

void edit_file() {

}

struct cfg_token *init_editor(struct cfg_token *token) {
        return token;
}