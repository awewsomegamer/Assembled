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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <util.h>
#include <pwd.h>
#include <unistd.h>

uint64_t general_hash(char *string) {
        uint64_t hash = 5381;

        int c;
        while ((c = *string++)) {
                hash = ((hash << 5) + hash) + c;
        }
        
        return hash;
}

char *fpath2abs(char *path, int options) {
	if (*path == '/') {
		// Path is already absolute, ignore
		return strdup(path);
	}

	char *clean = path;

	// If the path begins with a tilde, remove it
	if (*path == '~') {
		clean = strdup(path + 1);
	}

	char *final = NULL;

	switch (options) {
	// Relative to user's current working directory
	case 0: {
		char *cwd = (char *)malloc(0x1000);
		memset(cwd, 0, 0x1000);
		
		if (getcwd(cwd, 0x1000) != cwd) {
			DEBUG_MSG("Unable to get current working directory (%s, %s)\n", path, clean);
			
			free(cwd);
			free(clean);

			break;
		}

		cwd[strlen(cwd)] = '/';
		strcat(cwd, clean);

		final = strndup(cwd, strlen(cwd) + 1);

		free(cwd);

		break;
	}

	// Relaative to root of configuration file
	case 1: {
		struct passwd *pwd = getpwuid(getuid());

		size_t size = strlen(pwd->pw_dir) + strlen("/.config/assembled/") + 1;
		final = (char *)malloc(size);
		memset(final, 0, size);

		strcat(final, pwd->pw_dir);
		strcat(final, "/.config/assembled/");
		
		break;
	}
	}

	return final;
}