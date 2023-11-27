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

#ifndef CONFIG_H
#define CONFIG_H

#include <global.h>

#define CFG_TOKEN_EOF 0 // ''
#define CFG_TOKEN_COL 1 // ':'
#define CFG_TOKEN_STR 2 // "Sdfsdfaioietuaeadfbvx"
#define CFG_TOKEN_INT 3 // 123452
#define CFG_TOKEN_COM 4 // ','
#define CFG_TOKEN_KEY 5 // keyword like "keyseq" or "use" or "themes" (any string in str_lookup)
#define CFG_TOKEN_SQR 6 // '[' ']'

#define NEXT_TOKEN token = token->next;

#define EXPECT_TOKEN(__type__, __msg__) \
                        if (token->type != __type__) { \
                                printf("Error on line %d, %d: %s\n", token->line, token->column, __msg__); \
                                DEBUG_MSG("Error on line %d, %d: %s\n", token->line, token->column, __msg__); \
                                exit(1); \
                        }
#define EXPECT_VALUE(__value__, __msg__) \
                        if (token->value != __value__) { \
                                printf("Error on line %d, %d: %s\n", token->line, token->column, __msg__); \
                                DEBUG_MSG("Error on line %d, %d: %s\n", token->line, token->column, __msg__); \
                                exit(1); \
                        }

// TODO: Optimize placement
enum STR_LOOKUP_IDXS {
        CFG_LOOKUP_KEYBOARD,
        CFG_LOOKUP_START_SCR,
        CFG_LOOKUP_THEMES,
        CFG_LOOKUP_USE,
        CFG_LOOKUP_BG_DISABLE,
        CFG_LOOKUP_KEYSEQ,
	CFG_LOOKUP_LOGO_BMP,
	CFG_LOOKUP_COLUMNS,
	CFG_LOOKUP_DEFAULT,
	CFG_LOOKUP_DEFINE,
	CFG_LOOKUP_INCLUDE,

	CFG_LOOKUP_UP,
	CFG_LOOKUP_DOWN,
	CFG_LOOKUP_LEFT,
	CFG_LOOKUP_RIGHT,
	CFG_LOOKUP_ENTER,
	CFG_LOOKUP_BUFFER_LEFT,
	CFG_LOOKUP_BUFFER_RIGHT,
	CFG_LOOKUP_WINDOW_LEFT,
	CFG_LOOKUP_WINDOW_RIGHT,
	CFG_LOOKUP_FILE_SAVE,
	CFG_LOOKUP_FILE_SAVE_ALL,
	CFG_LOOKUP_FILE_LOAD,
	CFG_LOOKUP_SELECTION,
};

static const char *str_lookup[] = {
        [CFG_LOOKUP_KEYBOARD]   = "keyboard",
        [CFG_LOOKUP_START_SCR]  = "start_screen",
        [CFG_LOOKUP_THEMES]     = "themes",
        [CFG_LOOKUP_USE]        = "use",
        [CFG_LOOKUP_BG_DISABLE] = "background_disable",
        [CFG_LOOKUP_KEYSEQ]     = "keyseq",
	[CFG_LOOKUP_LOGO_BMP]   = "logo_bmp",
	[CFG_LOOKUP_COLUMNS]  	= "columns",
	[CFG_LOOKUP_DEFAULT] 	= "default",
	[CFG_LOOKUP_DEFINE]	= "define",
	[CFG_LOOKUP_INCLUDE]	= "include",

	[CFG_LOOKUP_UP]            = "up",
	[CFG_LOOKUP_DOWN]          = "down",
	[CFG_LOOKUP_LEFT]          = "left",
	[CFG_LOOKUP_RIGHT]         = "right",
	[CFG_LOOKUP_ENTER]         = "enter",
	[CFG_LOOKUP_BUFFER_LEFT]   = "buffer_left",
	[CFG_LOOKUP_BUFFER_RIGHT]  = "buffer_right",
	[CFG_LOOKUP_WINDOW_LEFT]   = "window_left",
	[CFG_LOOKUP_WINDOW_RIGHT]  = "window_right",
	[CFG_LOOKUP_FILE_SAVE]     = "file_save",
	[CFG_LOOKUP_FILE_SAVE_ALL] = "file_save_all",
	[CFG_LOOKUP_FILE_LOAD]     = "file_load",
	[CFG_LOOKUP_SELECTION]     = "selection",
};

struct cfg_token {
        int type;
        int value;
        char *str;
        struct cfg_token *next;
        int line;
        int column;
};

struct cfg_token *cfg_lex(FILE *file);
int read_config();

#endif
