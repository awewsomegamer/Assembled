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

#include <interface/interface.h>
#include <util.h>

struct render_context current_render_context;
struct screen screens[MAX_SCREEN_COUNT];
struct screen *active_screen;

int switch_to_screen(char *name) {
        int i = GET_SCR_IDX(name);
        
        active_screen = &screens[i];

        return i;
}

int register_screen(char *name, void (*render)(struct render_context *), void (*update)(struct render_context *), void (*local)(int)) {
        int i = GET_SCR_IDX(name);

        screens[i].render = render;
        screens[i].update = update;
        screens[i].local = local;
        screens[i].name = name;

        return i;
}