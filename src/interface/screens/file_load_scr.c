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

#include <editor/config.h>
#include <editor/buffer/editor.h>

#include <interface/interface.h>
#include <interface/screens/file_load_scr.h>

#include <global.h>

static char *file_path = NULL;
static size_t size = 1;

static char **directory_listing = NULL;
static size_t directory_listing_size = 1;
static int directory_listing_offset = 0;

static void render(struct AS_RenderCtx *ctx) {
	mvprintw(1, (ctx->max_x / 4 + 1), "%s", (file_path == NULL ? "" : file_path));
	draw_border(ctx->max_x / 4, 0, ctx->max_x / 2, ctx->max_y - 1);
	draw_border(ctx->max_x / 4, 2, ctx->max_x / 2, ctx->max_y - 3);

	if (directory_listing != NULL) {
		for (int i = 0; i < ctx->max_y - 1; i++) {
			if (i + directory_listing_offset < directory_listing_size - 1) {
				mvprintw(i + 3, ctx->max_x / 4 + 1, "%s", directory_listing[i + directory_listing_offset]);
			}
		}
	}

	move(1, (ctx->max_x / 4) + size);
}

static void update_directory_listing(char *abs) {
	if (directory_listing != NULL) {
		for (int i = 0; i < directory_listing_size - 1; i++) {
			if (directory_listing[i] != NULL) {
				free(directory_listing[i]);
			}
		}

		free(directory_listing);
		directory_listing = (char **)calloc(1, sizeof(char *));
		directory_listing_size = 1;
	} else {
		directory_listing = (char **)calloc(1, sizeof(char *));
		directory_listing_size = 1;
	}

	DIR *dir = opendir(abs);

	if (dir == NULL) {
		return;
	}

	struct dirent *dirent = NULL;

	while ((dirent = readdir(dir)) != NULL) {
		directory_listing[directory_listing_size - 1] = strdup(dirent->d_name);
		directory_listing = (char **)realloc(directory_listing, ++directory_listing_size * sizeof(char *));
	}
	
	closedir(dir);
}

static void local(int code, int value) {
	switch (code) {
	case LOCAL_ENTER: {
		if (directory_listing_size == 1) {
			switch_to_last_screen();
		}

		struct stat *st = (struct stat *)malloc(sizeof(struct stat));
		char *abs = fpath2abs(file_path, 0);

		stat(abs, st);

		if (S_ISDIR(st->st_mode)) {
			update_directory_listing(abs);
		} else if (S_ISREG(st->st_mode)) {
			load_file(abs);
			switch_to_screen("editor");
		}
		
		AS_DEBUG_MSG("%s\n", abs);

		free(abs);
		
		break;
	}

	case LOCAL_BUFFER_CHAR: {
		if (file_path == NULL) {
			file_path = (char *)malloc(size + 1);
			*file_path = 0;
		}

		if (value == '\b' || value == 263) {
			if (size == 1) {
				break;
			}

			size--;
			file_path = (char *)realloc(file_path, size);
			file_path[size - 1] = 0;

			break;
		}

		file_path[size - 1] = (char)value;
		file_path = (char *)realloc(file_path, ++size);
		file_path[size - 1] = 0;

		break;
	}

	case LOCAL_ARROW_YMOVE: {
		if (directory_listing_offset <= directory_listing_size && directory_listing_offset >= 0) {
			directory_listing_offset += value;
			directory_listing_offset = min(max(0, directory_listing_offset), directory_listing_size);
		}
		
		break;
	}
	}
}

void register_file_load_scr() {
	AS_DEBUG_MSG("Initializing file load screen\n");

	int i = register_screen("file_load", render, NULL, local);
	as_ctx.screens[i].render_options |= SCR_OPT_ON_UPDATE;
}

struct AS_CfgTok *configure_file_load_scr(struct AS_CfgTok *token) {
	return token;
}
