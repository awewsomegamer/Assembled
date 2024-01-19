/**
 * @file config.c
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

#include <ctype.h>
#include <editor/keyboard.h>
#include <editor/config.h>
#include <editor/buffer/editor.h>

#include <interface/screens/start.h>
#include <interface/theming/themes.h>

#include <global.h>
#include <util.h>

#ifdef AS_GLIB_ENABLE
	#include <glib-2.0/glib.h>
	static GHashTable *lex_tokens;
#endif

/**
 * Interpret the token stream for configuration files.
 *
 * @param struct AS_CfgTok *token - The linked list of tokens to interpret.
 * */
static void interpret_token_stream(struct AS_CfgTok *token) {
        while (token->type != AS_CFG_TOKEN_EOF) {
                AS_EXPECT_TOKEN(AS_CFG_TOKEN_KEY, "Expected command (i.e. keyboard, themes, or start_screen)");
                int command = token->value;

                AS_NEXT_TOKEN

                switch (command) {
                case AS_CFG_LOOKUP_KEYBOARD: {
                        token = configure_keyboard(token);

                        break;
                }

                case AS_CFG_LOOKUP_START_SCR: {
                        token = configure_start_screen(token);
                        AS_NEXT_TOKEN

                        break;
                }

                case AS_CFG_LOOKUP_THEMES: {
                        token = configure_theme(token);
                        AS_NEXT_TOKEN

                        break;
                }

		case AS_CFG_LOOKUP_COLUMNS: {
			token = configure_editor(token);
			AS_NEXT_TOKEN

			break;
		}

		case AS_CFG_LOOKUP_INCLUDE: {
			AS_EXPECT_TOKEN(AS_CFG_TOKEN_STR, "Expected string");
			
			// Get the name of the home directory
			struct passwd *pw = getpwuid(getuid());

			char *file_path = token->str;
			
			if (*file_path != '/') {
				int length = strlen(pw->pw_dir) + strlen(token->str) + strlen("/.config/assembled/") + 1;
				file_path = (char *)malloc(length);
				memset(file_path, 0, length);

				strcat(file_path, pw->pw_dir);
				strcat(file_path + strlen(pw->pw_dir), "/.config/assembled/");
				strcat(file_path + strlen(pw->pw_dir) + 1, token->str);
			}

			FILE *file = fopen(file_path, "r");

			// Read new file and interpret it
			struct AS_CfgTok *new_stream = cfg_lex(file);
			interpret_token_stream(new_stream);

			AS_DEBUG_MSG("Token list:\n");
			while (new_stream != NULL) {
				AS_DEBUG_MSG("%d { 0x%02X, \"%s\", (%d, %d) } %p\n", new_stream->type, new_stream->value, new_stream->str, new_stream->line, new_stream->column, new_stream->next);
				struct AS_CfgTok *tmp = new_stream->next;

				free(new_stream);

				new_stream = tmp;
			}
			AS_DEBUG_MSG("List end\n");

			if (file_path != token->str) {
				free(file_path);
			}

			fclose(file);

			AS_NEXT_TOKEN
		}
                }
        }
}

// BUG?:  Trailing commas may cause errors if they are
//        are directly followed by a comment.
struct AS_CfgTok *cfg_lex(FILE *file) {
        struct AS_CfgTok *head = (struct AS_CfgTok *)malloc(sizeof(struct AS_CfgTok));
	memset(head, 0, sizeof(struct AS_CfgTok));
	
        int line = 1;
        int column = 1;
        char putback = 0;

        char c = 0;
        bool comment = 0;
        struct AS_CfgTok *current = head;
        
        while ((c = ((putback == 0) ? fgetc(file) : putback)) != EOF) {
                putback = 0;

                switch (c) {
                case '\r':
                case '\n': {
                        line++;
                        column = 1;
                        comment = 0;

                        continue;
                }

                case '#': {
                        comment = 1;

                        continue;
                }

		case '\t':
                case ' ': {
                        continue;
                }
                }

                if (comment) {
                        continue;
                }

                struct AS_CfgTok *next = (struct AS_CfgTok *)malloc(sizeof(struct AS_CfgTok));
		memset(next, 0, sizeof(struct AS_CfgTok));
		
                current->next = next;
                current->line = line;
                current->column = column;

                switch (c) {
		case '[':
		case ']': {
			current->type = AS_CFG_TOKEN_SQR;
			column++;

			break;
		}

                case ':': {
                        current->type = AS_CFG_TOKEN_COL;
                        column++;

                        break;
                }

                case ',': {
                        current->type = AS_CFG_TOKEN_COM;
                        column++;

                        break;
                }

                case '\'': {
                        current->type = AS_CFG_TOKEN_INT;
                        current->value = fgetc(file);
                        fgetc(file);
                        column += 2;

                        break;
                }

                default: {
                        // Initialize a string
                        int size = 1;
                        char *str = (char *)malloc(size);

			// Parse the string with characters that are: '.', '/', '_', and alphanumeric
                        do {
                                column++;

                                if (isblank(c)) {
                                        continue;
                                }

                                *(str + (size - 1)) = c;
                                size++;
                                str = (char *)realloc(str, size * sizeof(char));
                        } while (((c = fgetc(file)) != EOF) && (isalnum(c) || c == '.' || c == '/' || c == '_'));

			// Putback last read char
                        putback = c;

			// Zero terminate string
                        *(str + size - 1) = 0;

                        // Number
                        if (isdigit(*str)) {
                                current->type = AS_CFG_TOKEN_INT;

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
                        current->type = AS_CFG_TOKEN_STR;
                        current->str = str;

                        // Keyword
                        #ifdef AS_GLIB_ENABLE
				int str_index = (uintptr_t)g_hash_table_lookup(lex_tokens, str) - 1;

				if (str_index >= 0) {
					current->type = AS_CFG_TOKEN_KEY;
					current->value = str_index;
					current->str = NULL;

					// String is no longer needed
					// Memory Manage
					free(str);
				}
			#else
				for (int i = 0; i < sizeof(As_LexStrLookup)/sizeof(As_LexStrLookup[0]); i++) {
					if (*str == *As_LexStrLookup[i] && strcmp(As_LexStrLookup[i], str) == 0) {
						current->type = AS_CFG_TOKEN_KEY;
						current->value = i;
						current->str = NULL;

						// String is no longer needed
						// Memory Manage
						free(str);

						break;
					}
				}
			#endif

                        break;
                }
                }

                current = next;
        }

        return head;
}

int read_config() {
	#ifdef AS_GLIB_ENABLE
		lex_tokens = g_hash_table_new(g_str_hash, g_str_equal);

		for (int i = 0; i < sizeof(As_LexStrLookup)/sizeof(As_LexStrLookup[0]); i++) {
			g_hash_table_insert(lex_tokens, (gpointer)As_LexStrLookup[i], (gpointer)(i + 1));
		}
	#endif

        // Get the name of the home directory
        struct passwd *pw = getpwuid(getuid());

        // Create absolute path to user configuration file
        char *path = (char *)malloc(strlen(pw->pw_dir) + strlen("/.config/assembled/config.cfg") + 1);
        strcpy(path, pw->pw_dir);
        strcpy(path + strlen(pw->pw_dir), "/.config/assembled/config.cfg");
        
        FILE *file = fopen(path, "r");

        if (file == NULL) {
                printf("Could not open file %s\n", path);
                AS_DEBUG_MSG("Could not open file %s\n", path);

                exit(1);
        }

        struct AS_CfgTok *head = cfg_lex(file);

        interpret_token_stream(head);

        AS_DEBUG_MSG("Token list:\n");
        struct AS_CfgTok *current = head;
        while (current != NULL) {
                AS_DEBUG_MSG("%d { 0x%02X, \"%s\", (%d, %d) } %p\n", current->type, current->value, current->str, current->line, current->column, current->next);
                struct AS_CfgTok *tmp = current->next;

                free(current);

                current = tmp;
        }
        AS_DEBUG_MSG("List end\n");

        // Memory Manage
        free(path);
        fclose(file);

        return 0;
}
