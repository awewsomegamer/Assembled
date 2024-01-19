/**
 * @file themes.c
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
 * Responsible for handling parsing of theme files, and initializing them into
 * ncurses.
*/

#include <editor/config.h>

#include <interface/theming/themes.h>

#include <global.h>
#include <ncurses.h>

/**
 * Defines a single color
 * */
struct AS_Color {
	/// The ARGB hex color value.
        uint32_t color_value;
	/**
	 * 0 0 0 0 0 0 F P\n
	 * P, 1: Color is present, 0: Color is not present.\n
	 * F, 1: Color is in the foreground, 0: Color is in the background.
	 * */
        uint8_t information;
};

/**
 * The colors of the current theme.
 * */
static struct AS_Color custom_colors[32];

// TODO: Allow users to set both foreground and background
//       color in the same line. Possibly even add a small
//       hash to the mix to allow users to use default colors
//       or predefined colors such as "blue" instead of
//       0x0000FF
/**
 * Read themes file found by configuration function.
 *
 * This function reads the color values in the themes file
 * found by the configuration function. Themes will overwrite
 * each other, but not clear the custom_colors array.
 *
 * @param FILE *file - The theme.cfg file.
 * */
static void read_theme(FILE *file) {
        struct AS_CfgTok *token = cfg_lex(file);
        struct AS_CfgTok *current = token;

        while (token->type != AS_CFG_TOKEN_EOF) {
                AS_EXPECT_TOKEN(AS_CFG_TOKEN_INT, "Expected integer");
                int idx = token->value;

                AS_NEXT_TOKEN
                AS_EXPECT_TOKEN(AS_CFG_TOKEN_COL, "Expected colon");

                AS_NEXT_TOKEN
                AS_EXPECT_TOKEN(AS_CFG_TOKEN_INT, "Expected integer");
                uint32_t color = token->value;

                AS_NEXT_TOKEN

		if (token->type == AS_CFG_TOKEN_COL) {
			AS_NEXT_TOKEN
			AS_EXPECT_TOKEN(AS_CFG_TOKEN_KEY, "Expected keyword")

			if (token->value == AS_CFG_LOOKUP_FOREGROUND) {
				custom_colors[idx].information |= (1 << 1);
			}

			AS_DEBUG_MSG("Funny moment\n")

			AS_NEXT_TOKEN
		}

		if (idx >= 32) {
                        continue;
		}

                custom_colors[idx].color_value = color;
                custom_colors[idx].information |= 1;
        }

        AS_DEBUG_MSG("Token list:\n");
	// TODO: Make linked list freeing a macro
        while (current != NULL) {
                AS_DEBUG_MSG("%d { 0x%02X, \"%s\", (%d, %d) } %p\n", current->type, current->value, current->str, current->line, current->column, current->next);
		struct AS_CfgTok *tmp = current->next;

                free(current);

                current = tmp;
        }
        AS_DEBUG_MSG("List end\n");
}

void register_custom_colors() {
        for (int i = 0; i < AS_MAX_CUSTOM_COLORS; i++) {
                if ((custom_colors[i].information & 1) == 0) {
                        continue;
		}

                short red   = (short)(((float)((custom_colors[i].color_value >> AS_RED_MASK  ) & 0xFF) / 256) * 1000);
                short green = (short)(((float)((custom_colors[i].color_value >> AS_GREEN_MASK) & 0xFF) / 256) * 1000);
                short blue  = (short)(((float)((custom_colors[i].color_value >> AS_BLUE_MASK ) & 0xFF) / 256) * 1000);

                init_color(i + AS_CUSTOM_COLOR_START, red, green, blue);

		if (((custom_colors[i].information >> 1) & 1) == 1) {
			init_pair(i + AS_CUSTOM_COLOR_START, i + AS_CUSTOM_COLOR_START, -1);
		} else {
			init_pair(i + AS_CUSTOM_COLOR_START, COLOR_BLACK, i + AS_CUSTOM_COLOR_START);
		}
        }
}

struct AS_CfgTok *configure_theme(struct AS_CfgTok *token) {
        AS_EXPECT_TOKEN(AS_CFG_TOKEN_KEY, "Expected keyword")

        switch (token->value) {
        case AS_CFG_LOOKUP_USE: {
                AS_NEXT_TOKEN
                AS_EXPECT_TOKEN(AS_CFG_TOKEN_COL, "Expected colon")
                AS_NEXT_TOKEN
                AS_EXPECT_TOKEN(AS_CFG_TOKEN_STR, "Expected string")
                
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
