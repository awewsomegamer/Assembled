/**
 * @file themes.h
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
 * Responsible for handling parsing of theme files, and initializing them into
 * ncurses.
*/

#ifndef AS_THEMES_H
#define AS_THEMES_H

#include <editor/config.h>

#include <global.h>
#include <includes.h>

/// Maximum number of colors defined in a theme.
#define AS_MAX_CUSTOM_COLORS  32

/// Start of the first custom color pair (right after the 16 predefined ncurses colors).
#define AS_CUSTOM_COLOR_START 16
/// The color pair index of the highlight color (the first custom color).
#define AS_COLOR_HIGHLIGHT    AS_CUSTOM_COLOR_START

/// Bit mask for red.
#define AS_RED_MASK           16
/// Bit mask for green.
#define AS_GREEN_MASK         8
/// Bit mask for blue.
#define AS_BLUE_MASK          0

/**
 * Function to initialize all custom color pairs.
 *
 * Invoked from init_ncurses in main.c once ncurses has initialized colors.
 * */
void register_custom_colors();

/**
 * Configuration for themes.
 *
 * Currently used to state which theme to use. Only one theme
 * can be loaded at a time.
 *
 * @param struct AS_CfgTok *token - The token which starts the line of configuration.
 * @return A pointer to the last token in the line of configuration (caller must use TOKEN_NEXT
 * to advance to the start of the next line of configuration). */
struct AS_CfgTok *configure_theme(struct AS_CfgTok *token);

#endif
