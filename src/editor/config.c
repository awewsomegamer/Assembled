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

#include <editor/keyboard.h>
#include <global.h>
#include <interface/screens/start.h>
#include <interface/theming/themes.h>
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
int read_config() {
        DEBUG_MSG("Reading configuration file\n");

        struct passwd *pw = getpwuid(getuid());

        char *path = (char *)malloc(strlen(pw->pw_dir) + strlen("/.config/assembled/config.cfg") + 1);
        strcpy(path, pw->pw_dir);
        strcat(path, "/.config/assembled/config.cfg");
        
        FILE *file = fopen(path, "r");

        if (file == NULL) {
                printf("Cannot open configuration file at %s\n", path);
                return 1;
        }

        DEBUG_MSG("Successfully opened configuration file %s\n", path);

        char *line;
        size_t chars_read = 0;
        while (getline(&line, &chars_read, file) != -1) {
                if (*line == '#')
                        continue;

                if (*(line + strlen(line) - 1) == '\n')
                        *(line + strlen(line) - 1) = 0;

                int tab_idx = 0;
                for (; tab_idx < strlen(line) && (*(line + tab_idx) != '\t'); tab_idx++);

                char *command = (char *)malloc(tab_idx + 1);
                memset(command, 0, tab_idx + 1); 
                strncpy(command, line, tab_idx);

                DEBUG_MSG("\"%s\": %d \"%s\" %lu\n", line, tab_idx, command, general_hash(command));

                line += tab_idx + 1;

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
                }
                }

                free(command);
        }

        fclose(file);
        free(path);

        DEBUG_MSG("Successfully read configuration\n");

        return 0;
}