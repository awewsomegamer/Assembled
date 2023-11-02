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

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <global.h>
#include <util.h>

#define MAX_FUNCTION_COUNT 256
#define GET_FUNC_IDX(name) (general_hash(name) % MAX_FUNCTION_COUNT)

#define UP_IDX           ((5863882U) % MAX_FUNCTION_COUNT)
#define DOWN_IDX         ((6385159517U) % MAX_FUNCTION_COUNT)
#define LEFT_IDX         ((6385435568U) % MAX_FUNCTION_COUNT)
#define RIGHT_IDX        ((210726633827U) % MAX_FUNCTION_COUNT)
#define ENTER_IDX        ((210711410595U) % MAX_FUNCTION_COUNT)
#define BUFFER_LEFT_IDX  ((13868330938437825225U) % MAX_FUNCTION_COUNT)
#define BUFFER_RIGHT_IDX ((14933063199426253724U) % MAX_FUNCTION_COUNT)

extern void (*functions[MAX_FUNCTION_COUNT])();

void layer_down();

#endif