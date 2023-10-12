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

#define UP_IDX          ((5863882U) % MAX_FUNCTION_COUNT)
#define DOWN_IDX        ((6385159517U) % MAX_FUNCTION_COUNT)
#define LEFT_IDX        ((6385435568U) % MAX_FUNCTION_COUNT)
#define RIGHT_IDX       ((210726633827U) % MAX_FUNCTION_COUNT)
#define ENTER_IDX       ((210711410595U) % MAX_FUNCTION_COUNT)

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