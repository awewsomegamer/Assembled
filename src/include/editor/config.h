/**
 * @file config.h
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
 * Responsible for reading .cfg files.
*/

#ifndef AS_CONFIG_H
#define AS_CONFIG_H

#include <includes.h>

/// Empty character, considered EOF
#define AS_CFG_TOKEN_EOF 0 // ''
/// Colon character
#define AS_CFG_TOKEN_COL 1 // ':'
/// A string of alphanumeric characters.
#define AS_CFG_TOKEN_STR 2 // "Sdfsdfaioietuaeadfbvx"
/// An integer of any base.
#define AS_CFG_TOKEN_INT 3 // 123452
/// A comma character.
#define AS_CFG_TOKEN_COM 4 // ','
/// A keyword in the list AS
#define AS_CFG_TOKEN_KEY 5 // keyword like "keyseq" or "use" or "themes" (any string in As_LexStrLookup)
#define AS_CFG_TOKEN_SQR 6 // '[' ']'

#define AS_NEXT_TOKEN token = token->next;

#define AS_EXPECT_TOKEN(__type__, __msg__) \
                        if (token->type != __type__) { \
                                printf("Error on line %d, %d: %s\n", token->line, token->column, __msg__); \
                                AS_DEBUG_MSG("Error on line %d, %d: %s\n", token->line, token->column, __msg__); \
                                exit(1); \
                        }
#define AS_EXPECT_VALUE(__value__, __msg__) \
                        if (token->value != __value__) { \
                                printf("Error on line %d, %d: %s\n", token->line, token->column, __msg__); \
                                AS_DEBUG_MSG("Error on line %d, %d: %s\n", token->line, token->column, __msg__); \
                                exit(1); \
                        }

// TODO: Optimize placement
/**
 * Enum which dictates keyword content placement
 *
 * The following are used indices into the As_LexStrLookup list.
 * The first group of indices are shared between the configuration
 * language and the function member of AS_KeySeq structures.
 *
 *  The separation is visible in source code (src/include/editor/config.h:73). */
enum AS_LEXSTRLOOKUP_IDXS {
	AS_CFG_LOOKUP_UP,
	AS_CFG_LOOKUP_DOWN,
	AS_CFG_LOOKUP_LEFT,
	AS_CFG_LOOKUP_RIGHT,
	AS_CFG_LOOKUP_ENTER,
	AS_CFG_LOOKUP_BUFFER_LEFT,
	AS_CFG_LOOKUP_BUFFER_RIGHT,
	AS_CFG_LOOKUP_WINDOW_LEFT,
	AS_CFG_LOOKUP_WINDOW_RIGHT,
	AS_CFG_LOOKUP_FILE_SAVE,
	AS_CFG_LOOKUP_FILE_SAVE_ALL,
	AS_CFG_LOOKUP_FILE_LOAD,
	AS_CFG_LOOKUP_SELECTION,
	AS_CFG_LOOKUP_MOVE_LN_UP,
	AS_CFG_LOOKUP_MOVE_LN_DOWN,
	AS_CFG_LOOKUP_COLDESC_LEFT,
	AS_CFG_LOOKUP_COLDESC_RIGHT,

        AS_CFG_LOOKUP_KEYBOARD,
        AS_CFG_LOOKUP_START_SCR,
        AS_CFG_LOOKUP_THEMES,
        AS_CFG_LOOKUP_USE,
        AS_CFG_LOOKUP_BG_DISABLE,
        AS_CFG_LOOKUP_KEYSEQ,
	AS_CFG_LOOKUP_LOGO_BMP,
	AS_CFG_LOOKUP_COLUMNS,
	AS_CFG_LOOKUP_DEFAULT,
	AS_CFG_LOOKUP_DEFINE,
	AS_CFG_LOOKUP_INCLUDE,
	AS_CFG_LOOKUP_FOREGROUND,
	AS_CFG_LOOKUP_BACKGROUND,
};

/**
 * Keywords in the configuration language.
 * */
static const char *As_LexStrLookup[] = {
	[AS_CFG_LOOKUP_UP]            	= "up",
	[AS_CFG_LOOKUP_DOWN]          	= "down",
	[AS_CFG_LOOKUP_LEFT]          	= "left",
	[AS_CFG_LOOKUP_RIGHT]         	= "right",
	[AS_CFG_LOOKUP_ENTER]         	= "enter",
	[AS_CFG_LOOKUP_BUFFER_LEFT]   	= "buffer_left",
	[AS_CFG_LOOKUP_BUFFER_RIGHT]  	= "buffer_right",
	[AS_CFG_LOOKUP_WINDOW_LEFT]   	= "window_left",
	[AS_CFG_LOOKUP_WINDOW_RIGHT]  	= "window_right",
	[AS_CFG_LOOKUP_FILE_SAVE]     	= "file_save",
	[AS_CFG_LOOKUP_FILE_SAVE_ALL] 	= "file_save_all",
	[AS_CFG_LOOKUP_FILE_LOAD]     	= "file_load",
	[AS_CFG_LOOKUP_SELECTION]     	= "selection",
	[AS_CFG_LOOKUP_MOVE_LN_UP]    	= "move_line_up",
	[AS_CFG_LOOKUP_MOVE_LN_DOWN]  	= "move_line_down",
	[AS_CFG_LOOKUP_COLDESC_LEFT]    = "column_left",
	[AS_CFG_LOOKUP_COLDESC_RIGHT]   = "column_right",

        [AS_CFG_LOOKUP_KEYBOARD]   	= "keyboard",
        [AS_CFG_LOOKUP_START_SCR]  	= "start_screen",
        [AS_CFG_LOOKUP_THEMES]     	= "themes",
        [AS_CFG_LOOKUP_USE]        	= "use",
        [AS_CFG_LOOKUP_BG_DISABLE] 	= "background_disable",
        [AS_CFG_LOOKUP_KEYSEQ]     	= "keyseq",
	[AS_CFG_LOOKUP_LOGO_BMP]	= "logo_bmp",
	[AS_CFG_LOOKUP_COLUMNS]  	= "columns",
	[AS_CFG_LOOKUP_DEFAULT] 	= "default",
	[AS_CFG_LOOKUP_DEFINE]		= "define",
	[AS_CFG_LOOKUP_INCLUDE]		= "include",
	[AS_CFG_LOOKUP_FOREGROUND]      = "foreground",
	[AS_CFG_LOOKUP_BACKGROUND]      = "background",
};

/**
 * Internal structure for representing the content of configuration files
 * */
struct AS_CfgTok {
	/// The type of the token (AS_CFG_TOKEN_\a x).
        int type;
	/// An integral value if the token has one.
        int value;
	/// The line number the token was found on.
        int line;
	/// The column number the token was found on.
        int column;
	/// String for tokens of type AS_CFG_TOKEN_STR.
        char *str;
	/// Pointer to the next token.
        struct AS_CfgTok *next;
};

/**
 * General lex function that can be used for .cfg files.
 *
 * @param FILE *file - The file to lex.
 * @return A pointer to the head of a struct AS_CfgTok linked list, the list is owned by the caller. */
struct AS_CfgTok *cfg_lex(FILE *file);

/**
 * Specific to read ~/.config/assembled/config.cfg
 *
 * This functions reads the first main configuration file
 * at ~/.config/assembled/config.cfg, which may point to other
 * configuration files.
 * @return An integer to determine success (0: success)*/
int read_config();

#endif
