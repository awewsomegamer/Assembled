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
#include "interface/interface.h"
#include <curses.h>
#include <editor/functions.h>

void layer_down() { }

static void cursor_up() {
        active_screen->local(LOCAL_ARROW_UP);
}

static void cursor_down() {
        active_screen->local(LOCAL_ARROW_DOWN);
}

static void cursor_left() {
        active_screen->local(LOCAL_ARROW_LEFT);
}

static void cursor_right() {
        active_screen->local(LOCAL_ARROW_RIGHT);
}

static void enter() {
        active_screen->local(LOCAL_ENTER);
}

void (*functions[MAX_FUNCTION_COUNT])() = {
        [UP_IDX] = cursor_up,
        [DOWN_IDX] = cursor_down,
        [LEFT_IDX] = cursor_left,
        [RIGHT_IDX] = cursor_right,
        [ENTER_IDX] = enter,
};