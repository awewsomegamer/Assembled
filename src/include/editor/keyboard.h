/**
 * @file keyboard.h
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
 * Responsible for handling all keyboard input and determining when the user
 * has entered a key sequence for the active screen to preform an action.
*/

#ifndef AS_KEYBOARD_H
#define AS_KEYBOARD_H

#include <editor/config.h>

#include <global.h>
#include <includes.h>

/// Maximum number of elements on the key_stack.
#define AS_MAX_KEY_ELEMENTS 16

/**
 * Acknowledge keyboard input.
 *
 * Place the given character onto the key_stack at key_stack_ptr.
 * Then try to collapse the stack.
 *
 * @param int c - Character to acknowledge.
 * */
void key(int c);

/**
 * Configure the keyboard.
 *
 * This function is invoked by config.c's interpret_token_stream
 * function. It is given the current element within within the
 * token list. The token must be of type AS_CFG_TOKEN_KEY with
 * a value of AS_CFG_LOOKUP_KEYBOARD.
 *
 * @param struct AS_CfgTok *token - The token from which to start interpretation.
 * @return A pointer to the beginning of the next set of tokens.
 * */
struct AS_CfgTok *configure_keyboard(struct AS_CfgTok *token);

#endif
