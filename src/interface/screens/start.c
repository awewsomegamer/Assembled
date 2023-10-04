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

#include "global.h"
#include "util.h"
#include <curses.h>
#include <interface/screens/start.h>
#include <interface/interface.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHARS_FROM_CENTER 32

static float t = 0.01;
static uint8_t dir = 0;
int background_enable = 1;

static const char *menu_table_left[] = {
        "Hello",
        "World this",
        "Is"
};

static const char *menu_table_right[] = {
        "A",
        "Test of",
        "My",
        "Menu system"
};

static const uint8_t logo_bmp[] = {
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x7F, 0xFF, 0xFF, 0xFF,
        0xF0, 0x0F, 0xFF, 0xC0, 0x3F, 0xFF, 0xFF, 0xFF, 0xE0, 0x1F, 0xFF, 0xE0, 0x1F, 0xFF, 0xFF, 0xFF, 0xC0, 0x3F, 0xFF, 0xE0, 0x0F, 0xFF, 0xFF, 0xFF, 0x80, 0x7F, 0xFF, 0xF0, 0x07, 0xFF, 0xFF,
        0xFF, 0x00, 0xFF, 0xFF, 0xF8, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x01, 0xFE, 0x00, 0x01, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x7F, 0x80, 0x03, 0xFF, 0xFF, 0xFF, 0x00, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x7F, 0xFF, 0xF0, 0x0F, 0xFF, 0xFF, 0xFF, 0xC0, 0x7F, 0xFF, 0xE0, 0x1F, 0xFF, 0xFF, 0xFF, 0xE0, 0x3F, 0xFF, 0xC0, 0x3F, 0xFF, 0xFF, 0xFF, 0xE0,
        0x1F, 0xFF, 0xC0, 0x7F, 0xFF, 0xFF, 0xFF, 0xF0, 0x0F, 0xFF, 0x80, 0x7F, 0xFF, 0xFF, 0xFF, 0xF8, 0x07, 0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0x03, 0xFE, 0x01, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFE, 0x01, 0xFC, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0xF8, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0xF8, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x70, 0x1F, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xC0, 0x20, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

void render(struct render_context *context) {
        if (background_enable) {
                for (int i = 0; i < max_y; i++) {
                        for (int j = 0; j < max_x; j++) {
                                int value = i * j * t * t;
                                int x = (value + j * i) % max_x;
                                int y = (value + i * j) % max_y;
                                
                                char c = (value <= 0xFF) ? ('0' + (x % 2)) : ' ';

                                mvprintw(y, x, "%c", c);

                        }
                }
        }

        // Draw Logo / Menu Bounding Bo
        int logo_y_offset = 0;
        int menu_y_offset = 0;
        if (max_y >= 60 && max_x >= 84) {
                logo_y_offset = (max_y / 4 - CHARS_FROM_CENTER / 2);
                menu_y_offset = CHARS_FROM_CENTER;

                int x = 0;
                int y = 39;
                for (int i = 0; i < 320; i++) {
                        for (int b = 7; b >= 0; b--) {
                                uint8_t bit = (logo_bmp[i] >> b) & 1;

                                if (bit) {
                                        attron(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));

                                        mvaddch(y + logo_y_offset, (max_x / 2 - CHARS_FROM_CENTER) + x, ' ');

                                        attroff(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));

                                }

                                x++;

                                if (x >= 64) {
                                        x = 0;
                                        y--;
                                }
                        }
                }
        } else {
                logo_y_offset = max_y / 2 - 2;
 
                attron(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));

                for (int i = 0; i < CHARS_FROM_CENTER / 4; i++) {
                        for (int j = 0; j < CHARS_FROM_CENTER * 2; j++) {
                                mvaddch(i + logo_y_offset, j + (max_x / 2) - (CHARS_FROM_CENTER), ' ');
                        }
                }
 
                attroff(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));
        }

        // Draw Options Menu
        for (int i = 0; i < sizeof(menu_table_left)/sizeof(menu_table_left[0]); i++) {
                mvprintw(logo_y_offset + 2 + menu_y_offset + i, (max_x / 2) - (strlen(menu_table_left[i]) / 2) - (CHARS_FROM_CENTER / 2), "%s", menu_table_left[i]);
        }

        for (int i = 0; i < sizeof(menu_table_right)/sizeof(menu_table_right[0]); i++) {
                mvprintw(logo_y_offset + 2 + menu_y_offset + i, (max_x / 2) - (strlen(menu_table_right[i]) / 2) + (CHARS_FROM_CENTER / 2), "%s", menu_table_right[i]);
        }
}

void update(struct render_context *context) {
        t += (dir ? -0.00005 : 0.00005);

        if (t >= 0.079) // 0.078
                dir = 1;
        if (t <= 0.01)
                dir = 0;
}

void register_start() {
        register_screen("start", render, update);
}

void configure_start_screen(char *line) {
        if (strcmp(line, "background_disable\n") == 0) {
                background_enable = 0;
        }

        if (strncmp(line, "logo_bmp", strlen("logo_bmp")) == 0) {
                printf("New logo file specified\n");
        }
}