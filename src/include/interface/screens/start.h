/**
 * @file start.h
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
 * The start screen that is displayed in the event no files are opened with the command.
*/

#ifndef AS_START_H
#define AS_START_H

#include <editor/config.h>

#include <includes.h>

/**
 * Function called by main to register editor screen.
 *
 * Wrapper of register_screen.
 * */
void register_start_screen();

/**
 * Configure the start screen.
 *
 * The start screen offers two types of configuration:
 * a change of logo and an option to turn off the binary
 * moving background.
 *
 * @param struct AS_CfgTok *token - The starting token of a line of configuration
 * for the screen.
 * @return A pointer to the last element of the line of configuration (caller must use NEXT_TOKEN
 * to move to the start of the next line).
 * */
struct AS_CfgTok *configure_start_screen(struct AS_CfgTok *token);

#endif
