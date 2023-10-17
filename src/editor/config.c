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

#include <ctype.h>
#include <editor/keyboard.h>
#include <global.h>
#include <interface/screens/start.h>
#include <interface/theming/themes.h>
#include <stdio.h>
#include <util.h>
#include <editor/config.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>

static struct token *head;
static int line = 1;
static int column = 1;
static char putback = 0;

void interpret_token_stream(struct token *token) {
        while (token->type != CFG_TOKEN_EOF) {
                EXPECT_TOKEN(CFG_TOKEN_KEY, "Expected command (i.e. keyboard, themes, or start_screen)");
                int command = token->value;

                NEXT_TOKEN
                EXPECT_TOKEN(CFG_TOKEN_TAB, "Expected tab")
                NEXT_TOKEN

                switch (command) {
                case CFG_LOOKUP_KEYBOARD: {
                        token = configure_keyboard(token);

                        break;
                }

                case CFG_LOOKUP_START_SCR: {
                        token = configure_start_screen(token);
                        NEXT_TOKEN

                        break;
                }

                case CFG_LOOKUP_THEMES: {
                        token = configure_theme(token);
                        NEXT_TOKEN
                }
                }
        }
}

int read_config() {
        head = (struct token *)malloc(sizeof(struct token));

        // Get the name of the home directory
        struct passwd *pw = getpwuid(getuid());

        // Create absolute path to user configuration file
        char *path = (char *)malloc(strlen(pw->pw_dir) + strlen("/.config/assembled/config.cfg") + 1);
        strcpy(path, pw->pw_dir);
        strcpy(path + strlen(pw->pw_dir), "/.config/assembled/config.cfg");
        
        FILE *file = fopen(path, "r");

        if (file == NULL) {
                printf("Could not open file %s\n", path);
                DEBUG_MSG("Could not open file %s\n", path);

                exit(1);
        }

        char c = 0;
        struct token *current = head;
        while ((c = (putback == 0) ? fgetc(file) : putback) != EOF) {
                if (putback != 0)
                        putback = 0;

                if (c == '\n' || c == '\r') {
                        line++;
                        column = 0;
                        continue;
                }

                struct token *next = (struct token *)malloc(sizeof(struct token));
                memset(next, 0, sizeof(struct token));

                current->next = next;

                current->line = line;
                current->column = column;

                switch (c) {
                case '\t': {
                        current->type = CFG_TOKEN_TAB;
                        column++;

                        break;
                }
                
                case ':': {
                        current->type = CFG_TOKEN_COL;
                        column++;

                        break;
                }

                case ',': {
                        current->type = CFG_TOKEN_COM;
                        column++;

                        break;
                }

                case '\'': {
                        current->type = CFG_TOKEN_INT;
                        current->value = fgetc(file);
                        fgetc(file);
                        column += 3;

                        break;
                }

                default: {
                        // Read string
                        int size = 1;
                        char *str = (char *)malloc(size);

                        do {
                                column++;

                                if (isblank(c))
                                        continue;

                                *(str + (size - 1)) = c;
                                size++;
                                str = (char *)realloc(str, size * sizeof(char));
                        } while (((c = fgetc(file)) != EOF) && (isalnum(c) || c == '.' || c == '/' || c == '_'));
                        
                        putback = c;

                        *(str + size - 1) = 0;

                        // Number
                        if (isdigit(*str)) {
                                current->type = CFG_TOKEN_INT;

                                if (*str == '0' && tolower(*(str + 1)) == 'x') {
                                        // Base 16
                                        current->value = strtol(str, NULL, 16);
                                } else if (*str == '0') {
                                        // Base 8
                                        current->value = strtol(str, NULL, 8);
                                } else {
                                        // Base 10
                                        current->value = strtol(str, NULL, 10);
                                }

                                // String is no longer needed
                                // Memory Manage
                                free(str);

                                break;
                        }

                        // String
                        current->type = CFG_TOKEN_STR;
                        current->str = str;

                        // Keyword
                        for (int i = 0; i < sizeof(str_lookup)/sizeof(str_lookup[0]); i++) {
                                if (strcmp(str_lookup[i], str) == 0) {
                                        current->type = CFG_TOKEN_KEY;
                                        current->value = i;
                                        current->str = NULL;

                                        // String is no longer needed
                                        // Memory Manage
                                        free(str);

                                        break;
                                }
                        }

                        break;
                }
                }

                current = next;
        }

        current = head;
        DEBUG_MSG("Token list:\n");
        while (current != NULL) {
                DEBUG_MSG("%d { %d, \"%s\", (%d, %d) } %p\n", current->type, current->value, current->str, current->line, current->column, current->next);

                current = current->next;
        }
        DEBUG_MSG("List end\n");

        free(path);
        fclose(file);

        interpret_token_stream(head);

        return 0;
}
