/*   Hazard Engine - 2D Game engine in C
 *   Copyright (C) 2025 Matthew Gobbi
 *   hazard-build.c - Level editor header
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

#ifndef HAZARD_BUILD_H
#define HAZARD_BUILD_H

#include "hazard-engine.h"

typedef enum build_mode {
	NONE,
	TILE,
	SPRITE
} build_mode;

void hbuild_editMap(haz_engine *e);
bool hbuild_saveMap(haz_engine *e, const char *filename);

#endif //HAZARD_BUILD_H
