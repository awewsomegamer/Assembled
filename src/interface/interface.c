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

#include "global.h"
#include <curses.h>
#include <interface/interface.h>
#include <util.h>

struct render_context current_render_context;
struct screen screens[MAX_SCREEN_COUNT];
struct screen *active_screen = NULL;
struct screen *last_screen = NULL;

int switch_to_screen(char *name) {
        int i = GET_SCR_IDX(name);

	last_screen = active_screen;
        active_screen = &screens[i];

        DEBUG_MSG("Switched to screen %d (\"%s\")\n", i, name);

        return i;
}

int switch_to_last_screen() {
	int i = (int)(((uintptr_t)active_screen - (uintptr_t)screens) / sizeof(struct screen));

	struct screen *tmp = last_screen;
	last_screen = active_screen;
	active_screen = tmp;

	DEBUG_MSG("Switched to last screen %d (\"%s\")\n", i, screens[i].name);

	return i;
}

int register_screen(char *name, void (*render)(struct render_context *), void (*update)(struct render_context *), void (*local)(int, int)) {
        int i = GET_SCR_IDX(name);

        screens[i].render = render;
        screens[i].update = update;
        screens[i].local = local;
        screens[i].name = name;

        DEBUG_MSG("Registered screen \"%s\" at position %d (0x%X, 0x%X, 0x%X)\n", name, i, render, update, local);

        return i;
}

void draw_border(int x, int y, int width, int height) {
	for (int i = y; i < (y + height); i++) {
		mvaddch(i, x, '|');
		mvaddch(i, x + width, '|');
	}
	
	for (int i = x; i < (x + width); i++) {
		mvaddch(y, i, '-');
		mvaddch(y + height, i, '-');
	}

	mvaddch(y, x, '+');
	mvaddch(y + height, x, '+');
	mvaddch(y, x + width, '+');
	mvaddch(y + height, x + width, '+');
}