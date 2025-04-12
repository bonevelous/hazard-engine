/*   Hazard Engine - 2D Game engine in C
 *   Copyright (C) 2025 Matthew Gobbi
 *   hazard-build.c - Level editor
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

#ifndef HAZARD_ACTOR_H
#define HAZARD_ACTOR_H

#include "hazard-engine.h"

typedef enum haz_actorFlag {
	ACTOR_NONE,
	ACTOR_PLAYER
} haz_actorFlag;

typedef struct haz_collision {
	bool left;
	bool right;
	bool top;
	bool bottom;
} haz_collision;

typedef struct haz_actor {
	haz_actorFlag flag;
	SDL_Texture *sprite;
	SDL_FRect frame;
	SDL_FRect position;
	SDL_Point speed;
	haz_collision collision;
} haz_actor;

void haz_actorUpdate(haz_engine *e, haz_actor *a);
void haz_actorCollideRect(haz_actor *a, SDL_FRect r);

#endif //HAZARD_ACTOR_H
