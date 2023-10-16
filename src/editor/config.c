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

// TODO: Implement a system where when an error
//       is recognized, the program doesn't instantly
//       close. Rather the whole of configuration is
//       read, and if the "ignore_errors" property
//       is enabled, then we continue on the best
//       we can.
// Return Value - Number of extraneous lines
int read_config() {
        int extraneous = 0;

        DEBUG_MSG("Reading configuration file\n");

        // Get the name of the home directory
        struct passwd *pw = getpwuid(getuid());

        // Create absolute path to user configuration file
        char *path = (char *)malloc(strlen(pw->pw_dir) + strlen("/.config/assembled/config.cfg") + 1);
        strcpy(path, pw->pw_dir);
        strcat(path, "/.config/assembled/config.cfg");
        
        FILE *file = fopen(path, "r");

        if (file == NULL) {
                printf("Cannot open configuration file at %s\n", path);
                return 1;
        }

        DEBUG_MSG("Successfully opened configuration file %s\n", path);

        // Read opened configuration file
        char *line;
        size_t chars_read = 0;
        while (getline(&line, &chars_read, file) != -1) {
                // Line starts with #, comment, continue to next line
                if (*line == '#')
                        continue;

                // Remove extraneous characters at the end of the line
                for (int i = strlen(line) - 1; i >= 0; i--) {
                        if (*(line + i) < 32)
                                *(line + i) = 0;
                        else
                                break;
                }

                // Find the index of the tab separating coommand and argument
                int tab_idx = 0;
                for (; tab_idx < strlen(line) && (*(line + tab_idx) != '\t'); tab_idx++);

                // Parse command
                char *command = (char *)malloc(tab_idx + 1);
                memset(command, 0, tab_idx + 1); 
                strncpy(command, line, tab_idx);

                DEBUG_MSG("\"%s\": %d \"%s\" %lu\n", line, tab_idx, command, general_hash(command));

                // Advance line pointer to
                line += tab_idx + 1;

                // Hash the command, compare it against existing hashes:
                // Match - Call the appropriate configuration function with the line.
                // No matches - Increment return value
                switch (general_hash(command)) {
                case CFG_CMD_KEYBOARD_HASH: {
                        configure_keyboard(line);

                        break;
                }

                case CFG_CMD_START_SCR_HASH: {
                        configure_start_screen(line);
                        
                        break;
                }

                case CFG_CMD_THEME_HASH: {
                        configure_theme(line);

                        break;
                }

                default: {
                        DEBUG_MSG("Line is extraneous!\n");
                        extraneous++;
                        
                        break;
                }
                }

                // Memory Manage
                free(command);
        }

        // Memory Manage
        fclose(file);
        free(path);

        DEBUG_MSG("Successfully read configuration\n");

        return extraneous;
}

// Return Value - Index at which the line ends
int read_line_section(char *line, char c) {
        int len = strlen(line);
        int idx = 0;

        for (; (idx < len) && ((*(line + idx) != c)); idx++)
                if (isblank(*(line + idx)))
                        return -1;

        return idx;
}