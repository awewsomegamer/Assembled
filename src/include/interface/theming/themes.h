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

#ifndef AS_THEMES_H
#define AS_THEMES_H

#include <editor/config.h>

#include <global.h>
#include <includes.h>

#define AS_MAX_CUSTOM_COLORS  32

#define AS_CUSTOM_COLOR_START 16
#define AS_COLOR_HIGHLIGHT    AS_CUSTOM_COLOR_START

#define AS_RED_MASK           16
#define AS_GREEN_MASK         8
#define AS_BLUE_MASK          0


void register_custom_colors();
struct AS_CfgTok *configure_theme(struct AS_CfgTok *token);

#endif
