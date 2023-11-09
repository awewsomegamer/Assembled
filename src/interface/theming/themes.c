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

#include <editor/config.h>
#include <curses.h>
#include <global.h>
#include <interface/theming/themes.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct assembled_color {
        uint32_t color_value;
        uint8_t information; // 0 0 0 0 0 0 0 P
                             //               ` 1: Color is non-null
};
static struct assembled_color custom_colors[32];

static void read_theme(FILE *file) {
        struct cfg_token *token = cfg_lex(file);
        struct cfg_token *current = token;

        while (token->type != CFG_TOKEN_EOF) {
                EXPECT_TOKEN(CFG_TOKEN_INT, "Expected integer");
                int idx = token->value;

                NEXT_TOKEN
                EXPECT_TOKEN(CFG_TOKEN_COL, "Expected colon");

                NEXT_TOKEN
                EXPECT_TOKEN(CFG_TOKEN_INT, "Expected integer")
                uint32_t color = token->value;

                NEXT_TOKEN

                if (idx >= 32)
                        continue;

                custom_colors[idx].color_value = color;
                custom_colors[idx].information |= 1;
        }

        DEBUG_MSG("Token list:\n");
        while (current != NULL) {
                DEBUG_MSG("%d { 0x%02X, \"%s\", (%d, %d) } %p\n", current->type, current->value, current->str, current->line, current->column, current->next);
		struct cfg_token *tmp = current->next;

                free(current);

                current = tmp;
        }
        DEBUG_MSG("List end\n");
}

void register_custom_colors() {
        for (int i = 0; i < MAX_CUSTOM_COLORS; i++) {
                if ((custom_colors[i].information & 1) == 0)
                        continue;

                short red =   (short)(((float)((custom_colors[i].color_value >> RED_MASK  ) & 0xFF) / 256) * 1000);
                short green = (short)(((float)((custom_colors[i].color_value >> GREEN_MASK) & 0xFF) / 256) * 1000);
                short blue =  (short)(((float)((custom_colors[i].color_value >> BLUE_MASK ) & 0xFF) / 256) * 1000);

                init_color(i + CUSTOM_COLOR_START, red, green, blue);
        }
}

struct cfg_token *configure_theme(struct cfg_token *token) {
        EXPECT_TOKEN(CFG_TOKEN_KEY, "Expected keyword")

        switch (token->value) {
        case CFG_LOOKUP_USE: {
                NEXT_TOKEN
                EXPECT_TOKEN(CFG_TOKEN_COL, "Expected colon")
                NEXT_TOKEN
                EXPECT_TOKEN(CFG_TOKEN_STR, "Expected string")
                
                char *path = token->str;

                // Not absolute path, assume user is relative to ~/.config/assembled/
                if (*path != '/') {
                        struct passwd *pw = getpwuid(getuid());
                        
                        path = (char *)malloc(strlen(pw->pw_dir) + strlen("/.config/assembled/") + strlen(token->str) + 1);
                        strcpy(path, pw->pw_dir);
                        strcat(path, "/.config/assembled/");
                        strcat(path, token->str);
                }

                FILE *theme_file = fopen(path, "r");

                if (theme_file == NULL) {
                        printf("Failed to open file %s\n", path);
                        exit(1);
                }

                read_theme(theme_file);

                fclose(theme_file);

                break;
        }
        }

        return token;
}