/**
 * @file editor_scr.h
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
 * The editor screen.
*/

#ifndef AS_EDITOR_SCR_H
#define AS_EDITOR_SCR_H

#include <editor/config.h>

#include <includes.h>

/**
 * Function called by main to register editor screen.
 *
 * Wrapper of register_screen.
 * */
void register_editor_screen();

/**
 * Configure the editor screen.
 *
 * This function is invoked by config.c's interpret_token_stream
 * function.
 *
 * Currently there is no configuration for the editor screen, therefore
 * the parameter is just returnd.
 *
 * @param struct AS_CfgTok *token - Currently just returned.
 * @return Currently just the input parameter.
 * */
struct AS_CfgTok *configure_editor_screen(struct AS_CfgTok *token);

#endif
