/**
 * @file nasm.h
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
 * This is a syntax backend module.
*/

#ifndef AS_ASM_BACKEND_H
#define AS_ASM_BACKEND_H

#include <editor/buffer/buffer.h>
/**
 * Initialization function for the backend
 *
 * Will restructure the keywords array into a g_hash_table
 * if AS_GLIB_ENABLE is defined.
 * @param int i - The position the backend should place itself in as_ctx.syn_backends
 * */
void as_asm_backend_init(int i);

#endif
