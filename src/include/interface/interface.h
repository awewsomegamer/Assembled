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

#ifndef INTERFACE_H
#define INTERFACE_H

#include <global.h>

#define MAX_SCREEN_COUNT        256
#define GET_SCR_IDX(name)       (general_hash(name) % MAX_SCREEN_COUNT)

#define LOCAL_ARROW_UP          0
#define LOCAL_ARROW_DOWN        1
#define LOCAL_ARROW_LEFT        2
#define LOCAL_ARROW_RIGHT       3
#define LOCAL_ENTER             4
#define LOCAL_LINE_INSERT	5
#define LOCAL_LINE_DELETION	6
#define LOCAL_BUFFER_LEFT	7
#define LOCAL_BUFFER_RIGHT	8
#define LOCAL_BUFFER_CHAR	9
#define LOCAL_WINDOW_LEFT	10
#define LOCAL_WINDOW_RIGHT	11

struct bound {
        int x;                  // Starting X
        int y;                  // Starting Y
        int w;                 // Width
        int h;                 // Height
};
// Determine if given coordinate is inside given bounding box
#define IN_BOUND(x, y, bound) \
        (x >= bound.x && x <= bound.x + bound.w) && \
        (y >= bound.y && y <= bound.y + bound.h)

struct render_context {
        int max_x;
        int max_y;
};
extern struct render_context current_render_context;

struct screen {
        void (*render)(struct render_context *);
        void (*update)(struct render_context *);
        void (*local)(int, int);
        char *name;
};

extern struct screen *active_screen;

int switch_to_screen(char *name);
int switch_to_last_screen();
int register_screen(char *name, void (*render)(struct render_context *), void (*update)(struct render_context *), void (*local)(int, int));

void draw_border(int x, int y, int width, int height);

#endif