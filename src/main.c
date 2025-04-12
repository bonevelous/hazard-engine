/*   Hazard Engine - 2D Game engine in C
 *   Copyright (C) 2025 Matthew Gobbi
 *   hazard-engine.c - Engine Functions
 *
 *   This program is free software: you can redistribute it and/or modify it
 *   under the terms of the GNU General Public License as published by the
 *   Free Software Foundation, either version 3 of the License, or (at your
 *   option) any later version.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *   for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "hazard-engine.h"
#include <SDL3/SDL_main.h>

haz_engine engine;

int main(int argc, char **argv) {
	haz_windowSetup(&engine);

	if (!haz_init(&engine)) {
		printf("ERROR: haz_init() failed.\n");
		return 1;
	}

	while (engine.status == 1) {
		haz_process(&engine, 20);
	}

	haz_quit(&engine);

	return 0;
}
