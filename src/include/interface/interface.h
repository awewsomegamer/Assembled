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

#ifndef AS_INTERFACE_H
#define AS_INTERFACE_H

#define AS_MAX_SCREEN_COUNT        256
#define GET_SCR_IDX(name)       (general_hash(name) % AS_MAX_SCREEN_COUNT)

#define LOCAL_ARROW_YMOVE       0
#define LOCAL_ARROW_XMOVE       1
#define LOCAL_ENTER             2
#define LOCAL_LINE_INSERT	3
#define LOCAL_LINE_DELETION	4
#define LOCAL_BUFFER_MOVE       5
#define LOCAL_BUFFER_CHAR	6
#define LOCAL_WINDOW_MOVE	7
#define LOCAL_WINDOW_SELECTION  8
#define LOCAL_BUFFER_MOVE_LINE  9
#define LOCAL_FILE_LOAD         10
#define LOCAL_FILE_SAVE         11
#define LOCAL_COLDESC_SWITCH    12

// Determine if given coordinate is inside given bounding box
#define IN_BOUND(x, y, bound) \
        (x >= bound.x && x <= bound.x + bound.w) && \
        (y >= bound.y && y <= bound.y + bound.h)
#define SCR_OPT_ON_UPDATE (1 << 0)
#define SCR_OPT_ALWAYS    (1 << 1)

#include <includes.h>

struct AS_Bound {
        int x;                 // Starting X
        int y;                 // Starting Y
        int w;                 // Width
        int h;                 // Height
};

struct AS_RenderCtx {
        int max_x;
        int max_y;
};

struct AS_Screen {
        void (*render)(struct AS_RenderCtx *);
        void (*update)(struct AS_RenderCtx *);
        void (*local)(int, int);
	int render_options;
        char *name;
};

int switch_to_screen(char *name);
int switch_to_last_screen();
int register_screen(char *name, void (*render)(struct AS_RenderCtx *), void (*update)(struct AS_RenderCtx *), void (*local)(int, int));

void draw_border(int x, int y, int width, int height);

#endif
