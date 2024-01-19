/**
 * @file interface.h
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
 * The screen manager.
*/

#ifndef AS_INTERFACE_H
#define AS_INTERFACE_H

/// Maximum number of screens.
#define AS_MAX_SCREEN_COUNT        256
/// Retrieve the index of a screen in the hashmap.
#define GET_SCR_IDX(name)       (general_hash(name) % AS_MAX_SCREEN_COUNT)

/// Local function code to move the cursor in the Y axis.
#define LOCAL_ARROW_YMOVE       0
/// Local function code to move the cursor in the X axis.
#define LOCAL_ARROW_XMOVE       1
/// Local function code when the user has pressed the enter key.
#define LOCAL_ENTER             2
/// Local function code when a line has been inserted by buffer.c.
#define LOCAL_LINE_INSERT	3
/// Local function code when a line has been removed by buffer.c.
#define LOCAL_LINE_DELETION	4
/// Local function code when the user moves to another buffer.
#define LOCAL_BUFFER_MOVE       5
/// Local function code when the user inserts a character.
#define LOCAL_BUFFER_CHAR	6
/// Local function code when the user moves to another file.
#define LOCAL_WINDOW_MOVE	7
/// Local function when the user starts a selection.
#define LOCAL_WINDOW_SELECTION  8
/// Local function when the user moves a line.
#define LOCAL_BUFFER_MOVE_LINE  9
/// Local function when the user wants to load a file.
#define LOCAL_FILE_LOAD         10
/// Local function when the user wants to save the file.
#define LOCAL_FILE_SAVE         11
/// Local function code when the user wants to switch column descriptors (layouts).
#define LOCAL_COLDESC_SWITCH    12

/// Determine if given coordinate is inside given bounding box.
#define IN_BOUND(x, y, bound) \
        (x >= bound.x && x <= bound.x + bound.w) && \
        (y >= bound.y && y <= bound.y + bound.h)

/**
 * The screen should update only when there is new user input.
 *
 * This value is in AS_Screen.render_options.
 * */
#define SCR_OPT_ON_UPDATE (1 << 0)
/**
 * The screen should update always.
 *
 * This value is in AS_Screen.render_options.
 * */
#define SCR_OPT_ALWAYS    (1 << 1)

#include <includes.h>

/**
 * Boundary structure which describes a rectangle.
 * */
struct AS_Bound {
	/// Starting X.
        int x;
	/// Starting Y.
        int y;
	/// Width.
        int w;
	/// Height.
        int h;
};

/**
 * The render context which is passed to screen functions.
 *
 * Contains important rendering information about the current terminal.
 * */
struct AS_RenderCtx {
	/// Maximum X-position.
        int max_x;
	/// Maximum Y-position.
        int max_y;
};

/**
 * Screen structure
 *
 * Defines the attributes of a screen.
 * */
struct AS_Screen {
	/// Function to call to render the screen.
        void (*render)(struct AS_RenderCtx *);
	/// Function to call to update the screen.
        void (*update)(struct AS_RenderCtx *);
	/// Function to call to interrupt the screen with input.
        void (*local)(int, int);
	int render_options;
	/// ASCII name of the screen.
        char *name;
};

/**
 * Switch to the given screen
 *
 * Change as_ctx.screen to the screen found at the
 * given name in the screens hashmap.
 *
 * @param char *name - The name of the screen to switch to.
 * @return The index of the screen in the hashmap.
 * */
int switch_to_screen(char *name);

/**
 * Switches to the last screen.
 *
 * The function flips the as_ctx.screen and the internal
 * last_screen pointers.
 *
 * @return The index of the screen in the hashmap.
 * */
int switch_to_last_screen();

/**
 * Registers a screen in the hashmap.
 *
 * The following initializes an entry in the hashmap with
 * the given arguments. The index within the hashmap is
 * calculated using the name parameter.
 *
 * @param char *name - The string by which to calculate the index within the hashmap
 * @param void (*render)(struct AS_RenderCtx *) - The render function of the screen.
 * @param void (*update)(struct AS_RenderCtx *) - The update function of the screen.
 * @param void (*local)(int, int) - The local function of the screen.
 * */
int register_screen(char *name, void (*render)(struct AS_RenderCtx *), void (*update)(struct AS_RenderCtx *), void (*local)(int, int));

/**
 * Utility function to draw a border around a given boundary
 *
 * @param  struct AS_Bound bound - The boundary around which to draw the border.
 * */
void draw_border(struct AS_Bound bound);

#endif
