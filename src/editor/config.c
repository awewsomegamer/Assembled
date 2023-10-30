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

void interpret_token_stream(struct cfg_token *token) {
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

                        break;
                }
                }
        }
}

// TODO: Implement includes
// ERROR: Trailing commas may cause errors if they are
//        are directly followed by a comment.
struct cfg_token *cfg_lex(FILE *file) {
        struct cfg_token *head = (struct cfg_token *)malloc(sizeof(struct cfg_token));
        
        int line = 1;
        int column = 1;
        char putback = 0;

        char c = 0;
        bool comment = 0;
        struct cfg_token *current = head;
        
        while ((c = ((putback == 0) ? fgetc(file) : putback)) != EOF) {
                putback = 0;

                switch (c) {
                case '\r':
                case '\n': {
                        line++;
                        column = 0;
                        comment = 0;

                        continue;
                }

                case '#': {
                        comment = 1;

                        continue;
                }

                case ' ': {
                        continue;
                }
                }

                if (comment) {
                        continue;
                }

                struct cfg_token *next = (struct cfg_token *)malloc(sizeof(struct cfg_token));
                memset(next, 0, sizeof(struct cfg_token));

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
                        column += 2;

                        break;
                }

                default: {
                        // Read string
                        int size = 1;
                        char *str = (char *)malloc(size);

                        do {
                                column++;

                                if (isblank(c)) {
                                        continue;
                                }

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

        return head;
}

int read_config() {
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

        struct cfg_token *head = cfg_lex(file);

        interpret_token_stream(head);

        DEBUG_MSG("Token list:\n");
        struct cfg_token *current = head;
        while (current != NULL) {
                DEBUG_MSG("%d { 0x%02X, \"%s\", (%d, %d) } %p\n", current->type, current->value, current->str, current->line, current->column, current->next);
                struct cfg_token *tmp = current->next;

                free(current);

                current = tmp;
        }
        DEBUG_MSG("List end\n");

        free(path);
        fclose(file);

        return 0;
}
