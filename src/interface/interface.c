/**
 * @file main.c
 * @author awewsomegamer <awewsomegamer@gmail.com>
 *
 * @section LICENSE
 *
 * Assembled - Column based text editor
 * Copyright (C) 2023-2024 awewsomegamer
 *
 * This file is apart of Assembled.
 *
 * Assembled is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @section DESCRIPTION
 *
 * The main program file
*/
#include <interface/interface.h>

#include <global.h>
#include <util.h>

struct AS_Screen *last_screen = NULL;

// Switch to the given screen
int switch_to_screen(char *name) {
        int i = GET_SCR_IDX(name);

        last_screen = as_ctx.screen;
        as_ctx.screen = &as_ctx.screens[i];

        AS_DEBUG_MSG("Switched to screen %d (\"%s\")\n", i, name);

        return i;
}

// Switch to the last screen
int switch_to_last_screen() {
	int i = (int)(((uintptr_t)as_ctx.screen - (uintptr_t)as_ctx.screens) / sizeof(struct AS_Screen));

	struct AS_Screen *tmp = last_screen;
	last_screen = as_ctx.screen;
	as_ctx.screen = tmp;

	AS_DEBUG_MSG("Switched to last screen %d (\"%s\")\n", i, as_ctx.screens[i].name);

	return i;
}

// Create a new screen in the list of available screens
int register_screen(char *name, void (*render)(struct AS_RenderCtx *), void (*update)(struct AS_RenderCtx *), void (*local)(int, int)) {
        int i = GET_SCR_IDX(name);

        as_ctx.screens[i].render = render;
        as_ctx.screens[i].update = update;
        as_ctx.screens[i].local = local;
        as_ctx.screens[i].name = name;
        as_ctx.screens[i].render_options = 0;

        AS_DEBUG_MSG("Registered screen \"%s\" at position %d (0x%X, 0x%X, 0x%X)\n", name, i, render, update, local);

        return i;
}

// Draw an ASCII border (rectangle) on the current screen
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
