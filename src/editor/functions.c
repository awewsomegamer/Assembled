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

#include "editor/buffer/buffer.h"
#include "editor/config.h"
#include "interface/screens/editor_scr.h"
#include <editor/buffer/editor.h>
#include <global.h>
#include <interface/interface.h>
#include <curses.h>
#include <editor/functions.h>
#include <stdio.h>

void layer_down() { }

// TODO: Simplify LOCAL codes so that they are
//       grouped by function (i.e. LOCAL_ARROW
//       _UP, LOCAL_ARROW_DOWN, etc... -> LOCAL
//       _ARROW_MOVE)
static void cursor_up() {
        as_ctx.screen->local(LOCAL_ARROW_UP, 0);
	sprintf(as_ctx.editor_scr_message, "UP");
}

static void cursor_down() {
        as_ctx.screen->local(LOCAL_ARROW_DOWN, 0);
	sprintf(as_ctx.editor_scr_message, "DOWN");
}

static void cursor_left() {
        as_ctx.screen->local(LOCAL_ARROW_LEFT, 0);
	sprintf(as_ctx.editor_scr_message, "LEFT");
}

static void cursor_right() {
        as_ctx.screen->local(LOCAL_ARROW_RIGHT, 0);
	sprintf(as_ctx.editor_scr_message, "RIGHT");
}

static void enter() {
        as_ctx.screen->local(LOCAL_ENTER, 0);
	sprintf(as_ctx.editor_scr_message, "ENTER");
}

static void buffer_left() {
	as_ctx.screen->local(LOCAL_BUFFER_MOVE, -1);
	sprintf(as_ctx.editor_scr_message, "BUFFER LEFT");
}

static void buffer_right() {
	as_ctx.screen->local(LOCAL_BUFFER_MOVE, 1);
	sprintf(as_ctx.editor_scr_message, "BUFFER RIGHT");
}

static void file_save() {
	save_file(as_ctx.text_file);
	sprintf(as_ctx.editor_scr_message, "SAVED FILE");
}

static void file_save_all() {
	save_all();
	sprintf(as_ctx.editor_scr_message, "SAVED ALL");
}

static void file_load() {
	switch_to_screen("file_load");
	sprintf(as_ctx.editor_scr_message, "LOADED FILE");
}

static void window_left() {
	as_ctx.screen->local(LOCAL_WINDOW_LEFT, 0);
	sprintf(as_ctx.editor_scr_message, "WINDOW LEFT");
}

static void window_right() {
	as_ctx.screen->local(LOCAL_WINDOW_RIGHT, 0);
	sprintf(as_ctx.editor_scr_message, "WINDOW RIGHT");
}

static void selection() {
	as_ctx.screen->local(LOCAL_WINDOW_SELECTION, 1);
	sprintf(as_ctx.editor_scr_message, "SELECTION");
}

static void move_line_up() {
	as_ctx.screen->local(LOCAL_BUFFER_MOVE_LINE, 1);
	sprintf(as_ctx.editor_scr_message, "MOVE LINE UP");
}

static void move_line_down() {
	as_ctx.screen->local(LOCAL_BUFFER_MOVE_LINE, 0);
	sprintf(as_ctx.editor_scr_message, "MOVE LINE DOWN");
}

void (*functions[MAX_FUNCTION_COUNT])() = {
        [AS_CFG_LOOKUP_UP]            = cursor_up,
        [AS_CFG_LOOKUP_DOWN]          = cursor_down,
        [AS_CFG_LOOKUP_LEFT]          = cursor_left,
        [AS_CFG_LOOKUP_RIGHT]         = cursor_right,
        [AS_CFG_LOOKUP_ENTER]         = enter,
	[AS_CFG_LOOKUP_BUFFER_LEFT]   = buffer_left,
	[AS_CFG_LOOKUP_BUFFER_RIGHT]  = buffer_right,
	[AS_CFG_LOOKUP_WINDOW_LEFT]   = window_left,
	[AS_CFG_LOOKUP_WINDOW_RIGHT]  = window_right,
	[AS_CFG_LOOKUP_FILE_SAVE]     = file_save,
	[AS_CFG_LOOKUP_FILE_SAVE_ALL] = file_save_all,
	[AS_CFG_LOOKUP_FILE_LOAD]     = file_load,
	[AS_CFG_LOOKUP_SELECTION]     = selection,
	[AS_CFG_LOOKUP_MOVE_LN_UP]    = move_line_up,
	[AS_CFG_LOOKUP_MOVE_LN_DOWN]  = move_line_down,
};
