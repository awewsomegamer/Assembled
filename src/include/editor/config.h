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
#define CFG_TOKEN_TAB 1 // '\t'
#define CFG_TOKEN_COL 2 // ':'
#define CFG_TOKEN_STR 3 // "Sdfsdfaioietuaeadfbvx"
#define CFG_TOKEN_INT 4 // 123452
#define CFG_TOKEN_COM 5 // ','
#define CFG_TOKEN_KEY 6 // keyword like "keyseq" or "use" or "themes" (any string in str_lookup)

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
enum STR_LOOKUP_IDXS {
        CFG_LOOKUP_KEYBOARD,
        CFG_LOOKUP_START_SCR,
        CFG_LOOKUP_THEMES,
        CFG_LOOKUP_USE,
        CFG_LOOKUP_BG_DISABLE,
        CFG_LOOKUP_KEYSEQ,
	CFG_LOOKUP_LOGO_BMP,
};

static const char *str_lookup[] = {
        [CFG_LOOKUP_KEYBOARD]    = "keyboard",
        [CFG_LOOKUP_START_SCR]   = "start_screen",
        [CFG_LOOKUP_THEMES]      = "themes",
        [CFG_LOOKUP_USE]         = "use",
        [CFG_LOOKUP_BG_DISABLE]  = "background_disable",
        [CFG_LOOKUP_KEYSEQ]      = "keyseq",
	[CFG_LOOKUP_LOGO_BMP]    = "logo_bmp",
};

struct token {
        int type;
        int value;
        char *str;
        struct token *next;
        int line;
        int column;
};

int read_config();
int read_line_section(char *line, char c);

#endif