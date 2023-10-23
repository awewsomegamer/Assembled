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
#include <global.h>
#include <stdlib.h>

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

struct text_buffer *new_buffer(char *name) {
        struct text_buffer *buffer = (struct text_buffer *)malloc(sizeof(struct text_buffer));

        buffer->name = name;
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