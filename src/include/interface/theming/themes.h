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

#ifndef THEMES_H
#define THEMES_H

#include <global.h>
#include <editor/config.h>

#define MAX_CUSTOM_COLORS  32
#define CUSTOM_COLOR_START 16

#define RED_MASK           16
#define GREEN_MASK         8
#define BLUE_MASK          0

void register_custom_colors();
struct cfg_token *configure_theme(struct cfg_token *token);

#endif