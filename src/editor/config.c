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

#include "editor/keyboard.h"
#include "interface/screens/start.h"
#include <util.h>
#include <editor/config.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int read_config() {
        struct passwd *pw = getpwuid(getuid());
        char *path = strcat(pw->pw_dir, "/.config/assembled/config.cfg");
        
        FILE *file = fopen(path, "r");

        if (file == NULL) {
                printf("Cannot open configuration file at %s\n", path);
                return 1;
        }

        char *line;
        size_t chars_read = 0;
        while (getline(&line, &chars_read, file) != -1) {
                if (*line == '#')
                        continue;

                int tab_idx = 0;
                for (; tab_idx < strlen(line) && (*(line + tab_idx) != '\t'); tab_idx++);

                char *command = (char *)malloc(tab_idx);
                strncpy(command, line, tab_idx);

                switch (general_hash(command)) {
                case CFG_CMD_KEYBOARD_HASH: {
                        init_keyboard(line + tab_idx + 1);

                        break;
                }

                case CFG_CMD_START_SCR_HASH: {
                        configure_start_screen(line + tab_idx + 1);
                        
                        break;
                }

                default: {
                        printf("Could not make use of line: %s\n", line);
                }
                }

                free(command);
        }

        fclose(file);

        return 0;
}