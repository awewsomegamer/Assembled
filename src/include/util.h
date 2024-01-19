/**
 * @file util.h
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
 * File containing functions which aren't specific to any one file.
*/
#ifndef AS_UTIL_H
#define AS_UTIL_H

#include <includes.h>

/**
 * Create a 64-bit hash from the given string input
 *
 * @param char *string - a zero terminated string to hash
 * @return 64-bit hash
 * */
uint64_t general_hash(char *string);

/**
 * Create an absolute path from the given path
 *
 * @param char *path - Relative or absolute path. If the path is already absolute,
 * it will be returned in a new allocation.
 * @param int options - Options for where the abolsute path should begin from.
 * 0: The path will be absolute to the user's current working directory.
 * 1: The path will be relative to ~/.config/assembled/.
 * */
char *fpath2abs(char *path, int options);

#endif
