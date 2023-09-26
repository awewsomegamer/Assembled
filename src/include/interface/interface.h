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

#define MAX_SCREEN_COUNT 256

struct render_context {

};
extern struct render_context current_render_context;

struct screen {
        void (*render)(struct render_context *);
        void (*update)(struct render_context *);
        char *name;
};

extern struct screen *active_screen;

uint64_t get_screen_hash(char *name);
int get_screen(char *name);
int switch_to_screen(char *name);
int register_screen(char *name, void (*render)(struct render_context *), void (*update)(struct render_context *));

#endif