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

#include <global.h>
#include <util.h>
#include <curses.h>
#include <interface/screens/start.h>
#include <interface/interface.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CHARS_FROM_CENTER       32
#define BMP_WIDTH               64
#define BMP_HEIGHT              40
#define SCREEN_MIN_WIDTH        (BMP_WIDTH  + 20)
#define SCREEN_MIN_HEIGHT       (BMP_HEIGHT + 20)

#define UPDATE_TIME_MAX         0.07900
#define UPDATE_TIME_MIN         0.01000
#define UPDATE_TIME_TICK        0.00005

static float time = 0.01;
static bool direction = 0;
static bool background_enable = 1;

static int cx = 0;
static int cy = 0;

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

void test() {
        DEBUG_MSG("First entry\n");
}

static void (*menu_functions[2][10])() = {
        {test},
        {}
};

static uint8_t logo_bmp_data[] = {
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

static void render(struct render_context *context) {
        if (background_enable) {
                for (int i = 0; i < context->max_y; i++) {
                        for (int j = 0; j < context->max_x; j++) {
                                int value = i * j * time * time - j;
                                int x = (value + j * i) % context->max_x;
                                int y = (value + i * j) % context->max_y;
                                
                                char c = (value <= 0xFF) ? ('0' + (x % 2)) : ' ';

                                mvprintw(y, x, "%c", c);

                        }
                }
        }

        // Draw Logo / Menu Bounding Bo
        int logo_y_offset = 0;
        int menu_y_offset = 0;
        if (context->max_y >= SCREEN_MIN_HEIGHT && context->max_x >= SCREEN_MIN_WIDTH) {
                logo_y_offset = (context->max_y / 4 - CHARS_FROM_CENTER / 2);
                menu_y_offset = CHARS_FROM_CENTER;

                int x = 0;
                int y = BMP_HEIGHT - 1;
                for (int i = 0; i < 320; i++) {
                        for (int b = 7; b >= 0; b--) {
                                uint8_t bit = (logo_bmp_data[i] >> b) & 1;

                                if (bit) {
                                        attron(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));

                                        mvaddch(y + logo_y_offset, (context->max_x / 2 - CHARS_FROM_CENTER) + x, ' ');

                                        attroff(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));

                                }

                                x++;

                                if (x >= BMP_WIDTH) {
                                        x = 0;
                                        y--;
                                }
                        }
                }
        } else {
                logo_y_offset = context->max_y / 2 - 2;
 
                attron(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));

                for (int i = 0; i < CHARS_FROM_CENTER / 4; i++) {
                        for (int j = 0; j < CHARS_FROM_CENTER * 2; j++) {
                                mvaddch(i + logo_y_offset, j + (context->max_x / 2) - (CHARS_FROM_CENTER), ' ');
                        }
                }
 
                attroff(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));
        }

        // Draw Options Menu
        for (int i = 0; i < sizeof(menu_table_left)/sizeof(menu_table_left[0]); i++) {
                if (cy == i && cx == 0)
                        attron(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));

                mvprintw(logo_y_offset + 2 + menu_y_offset + i,
                        (context->max_x / 2) - (strlen(menu_table_left[i]) / 2) - (CHARS_FROM_CENTER / 2),
                        "%s", menu_table_left[i]);

                if (cy == i && cx == 0)
                        attroff(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));
        }

        for (int i = 0; i < sizeof(menu_table_right)/sizeof(menu_table_right[0]); i++) {
                if (cy == i && cx == 1)
                        attron(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));

                mvprintw(logo_y_offset + 2 + menu_y_offset + i,
                        (context->max_x / 2) - (strlen(menu_table_right[i]) / 2) + (CHARS_FROM_CENTER / 2), 
                        "%s", menu_table_right[i]);

                if (cy == i && cx == 1)
                        attroff(COLOR_PAIR(ASSEMBLED_COLOR_HIGHLIGHT));
        }
}

static void update(struct render_context *context) {
        time += (direction ? -UPDATE_TIME_TICK : UPDATE_TIME_TICK);
        
        int max_y = cx ? sizeof(menu_table_right)/sizeof(menu_table_right[0]) : sizeof(menu_table_left)/sizeof(menu_table_left[0]);
        if (cy >= max_y)
                cy = max_y - 1;

        if (time >= UPDATE_TIME_MAX)
                direction = 1;
        if (time <= UPDATE_TIME_MIN)
                direction = 0;
}

static void local(int code) {
        int max_y = cx ? sizeof(menu_table_right)/sizeof(menu_table_right[0]) : sizeof(menu_table_left)/sizeof(menu_table_left[0]);

        switch(code) {
        case LOCAL_ARROW_UP: {
                if (cy > 0)
                        cy--;

                break;
        }

        case LOCAL_ARROW_DOWN: {
                if (cy < max_y - 1)
                        cy++;

                break;
        }

        case LOCAL_ARROW_LEFT: {
                cx = 0;
                
                break;
        }

        case LOCAL_ARROW_RIGHT: {
                cx = 1;

                break;
        }

        case LOCAL_ENTER: {
                void (*func)() = menu_functions[cx][cy];

                if (func != NULL)
                        (*func)();

                break;
        }
        }
}

void register_start() {
        DEBUG_MSG("Registering start screen\n");
        register_screen("start", render, update, local);
}

struct token *configure_start_screen(struct token *token) {
        switch (token->type) {
        case CFG_LOOKUP_BG_DISABLE: {
                background_enable = 0;
                
                break;
        }

        case CFG_LOOKUP_LOGO_BMP: {
                NEXT_TOKEN
                EXPECT_TOKEN(CFG_TOKEN_COL, "Expected colon")
                NEXT_TOKEN
                EXPECT_TOKEN(CFG_TOKEN_STR, "Expected string")

                FILE *bmp = fopen(token->str, "r");

                if (bmp == NULL) {
                        printf("Could not open BMP file %s\n", token->str);
                        exit(1);
                }
                
                uint16_t identifier = 0;
                uint32_t data_offset = 0;
                uint32_t width = 0;
                uint32_t height = 0;
                uint16_t bpp = 0;

                fseek(bmp, 0x00, SEEK_SET);
                fread(&identifier, 2, 1, bmp);

                // "BM" in little endian
                if (identifier != 0x4D42) {
                        printf("%s is not a BMP\n", token->str);
                        exit(1);
                }

                fseek(bmp, 0x0A, SEEK_SET);
                fread(&data_offset, 4, 1, bmp);
                
                fseek(bmp, 0x12, SEEK_SET);
                fread(&width, 4, 1, bmp);
                
                fseek(bmp, 0x16, SEEK_SET);
                fread(&height, 4, 1, bmp);
                
                fseek(bmp, 0x1C, SEEK_SET);
                fread(&bpp, 2, 1, bmp);
                
                if (width != 64 || height != 40 || bpp != 1) {
                        printf("Image has incorrect dimensions (%dx%dx%d). Correct dimensions are 64x40x1", width, height, bpp);
                        exit(1);
                }

                fseek(bmp, data_offset, SEEK_SET);
                fread(logo_bmp_data, 1, 320, bmp);

                break;
        }
        }

        return token;
}
