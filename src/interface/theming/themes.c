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
                             //               ` Color is non-null
};
static struct assembled_color custom_colors[32];

static void read_lines(FILE *file) {
        char *line = NULL;
        size_t count = 0;
        while (getline(&line, &count, file) != -1) {
                char *end = line;
                for (; (end < (line + strlen(line))) && (*end != ':'); end++);
                int idx = strtol(line, &end, 10);
                
                if (idx >= MAX_CUSTOM_COLORS || idx < 0)
                        continue;

                uint32_t color = strtol(end + 1, NULL, 16);

                custom_colors[idx].color_value = color;
                custom_colors[idx].information |= 1;
        }
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

void configure_theme(char *line) {
        if (strncmp(line, "use", 3) != 0)
                return;

        line += 4;

        char *path = line;

        // Not absolute path, assume user is relative to ~/.config/assembled/
        if (*path != '/') {
                struct passwd *pw = getpwuid(getuid());
                
                path = (char *)malloc(strlen(pw->pw_dir) + strlen("/.config/assembled/") + strlen(line) + 1);
                strcpy(path, pw->pw_dir);
                strcat(path, "/.config/assembled/");
                strcat(path, line);
        }

        FILE *theme_file = fopen(path, "r");

        if (theme_file == NULL) {
                printf("Failed to open file %s\n", path);
                exit(1);
        }

        read_lines(theme_file);

        if (path != line)
                free(path);

        fclose(theme_file);

        DEBUG_MSG("%X\n", custom_colors[0].color_value);
}