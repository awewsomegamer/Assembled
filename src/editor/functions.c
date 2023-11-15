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

#include "editor/config.h"
#include "interface/screens/editor_scr.h"
#include <editor/buffer/editor.h>
#include <global.h>
#include <interface/interface.h>
#include <curses.h>
#include <editor/functions.h>

void layer_down() { }

static void cursor_up() {
	editor_scr_message = "UP";
        active_screen->local(LOCAL_ARROW_UP, 0);
}

static void cursor_down() {
	editor_scr_message = "DOWN";
        active_screen->local(LOCAL_ARROW_DOWN, 0);
}

static void cursor_left() {
	editor_scr_message = "LEFT";
        active_screen->local(LOCAL_ARROW_LEFT, 0);
}

static void cursor_right() {
	editor_scr_message = "RIGHT";
        active_screen->local(LOCAL_ARROW_RIGHT, 0);
}

static void enter() {
	editor_scr_message = "ENTER";
        active_screen->local(LOCAL_ENTER, 0);
}

static void buffer_left() {
	active_screen->local(LOCAL_BUFFER_LEFT, 0);
	editor_scr_message = "BUFFER LEFT";
}

static void buffer_right() {
	active_screen->local(LOCAL_BUFFER_RIGHT, 0);
	editor_scr_message = "BUFFER RIGHT";
}

static void file_save() {
	save_file(active_text_file);
	editor_scr_message = "SAVED FILE";
}

static void file_save_all() {
	save_all();
	editor_scr_message = "SAVED ALL";
}

static void file_load() {
	switch_to_screen("file_load");
	editor_scr_message = "LOADED FILE";
}

void (*functions[MAX_FUNCTION_COUNT])() = {
        [CFG_LOOKUP_UP] = cursor_up,
        [CFG_LOOKUP_DOWN] = cursor_down,
        [CFG_LOOKUP_LEFT] = cursor_left,
        [CFG_LOOKUP_RIGHT] = cursor_right,
        [CFG_LOOKUP_ENTER] = enter,
	[CFG_LOOKUP_BUFFER_LEFT] = buffer_left,
	[CFG_LOOKUP_BUFFER_RIGHT] = buffer_right,
	[CFG_LOOKUP_FILE_SAVE] = file_save,
	[CFG_LOOKUP_FILE_SAVE_ALL] = file_save_all,
	[CFG_LOOKUP_FILE_LOAD] = file_load,
};