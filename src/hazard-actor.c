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

#include "hazard-engine.h"
#include "hazard-actor.h"

void haz_actorUpdate(haz_engine *e, haz_actor *a) {
	const bool *key = SDL_GetKeyboardState(NULL);

	SDL_Point d = {0, 0};

	if (key[SDL_SCANCODE_LEFT] ^ key[SDL_SCANCODE_RIGHT]) {
		if (key[SDL_SCANCODE_LEFT] && !a->collision.left) d.x = -1;
		if (key[SDL_SCANCODE_RIGHT] && !a->collision.right) d.x = 1;
	}

	if (key[SDL_SCANCODE_UP] ^ key[SDL_SCANCODE_DOWN]) {
		if (key[SDL_SCANCODE_UP] && !a->collision.top) d.y = -1;
		if (key[SDL_SCANCODE_DOWN] && !a->collision.bottom) d.y = 1;
	}

	a->position.x += a->speed.x * d.x;
	a->position.y += a->speed.y * d.y;

	SDL_RenderTexture(e->renderer, a->sprite, &a->frame, &a->position);
}



void haz_actorCollideRect(haz_actor *a, SDL_FRect r) {
	haz_line _a = {
		a->position.x,
		a->position.x + a->position.w,
		a->position.y,
		a->position.y + a->position.h
	};

	haz_line _r = {r.x, r.y, r.x + r.w, r.y + r.h};

	a->collision.left = false;
	a->collision.right = false;
	for (int i = _a.y1; i < _a.y2; i++) {
		SDL_FPoint pl = {_a.x1 - 1, i};
		SDL_FPoint pr = {_a.x2, i};

		if (haz_pointInRect(pl, r)) { a->collision.left = true; }
		if (haz_pointInRect(pr, r)) { a->collision.right = true; }
	}

	a->collision.top = false;
	a->collision.bottom = false;
	for (int i = _a.x1; i < _a.x2; i++) {
		SDL_FPoint pt = {i, _a.y1 - 1};
		SDL_FPoint pb = {i, _a.y2};

		if (haz_pointInRect(pt, r)) {a->collision.top = true; }
		if (haz_pointInRect(pb, r)) { a->collision.bottom = true; }
	}
}
