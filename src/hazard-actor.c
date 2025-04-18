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
	a->rend.x = a->phys.pos.x;
	a->rend.y = a->phys.pos.y;

	haz_actorCollide(e, a);

	if ((a->phys.vel.x < 0 && !a->col.l) ^
		(a->phys.vel.x > 0 && !a->col.r)) {

		a->phys.pos.x += a->phys.vel.x;
	}

	if ((a->phys.vel.y < 0 && !a->col.t) ^
		(a->phys.vel.y > 0 && !a->col.b)) {

		a->phys.pos.y += a->phys.vel.y;
	}

	switch(a->flag) {
		case ACTOR_PLAYER:
			haz_player(e, a);
			break;
		default:
			break;
	}

	SDL_RenderTexture(e->ren, a->tex, &a->frame, &a->rend);
}

void haz_player(haz_engine *e, haz_actor *a) {
	const bool *key = SDL_GetKeyboardState(NULL);
	SDL_Point d = {0, 0};

	if (key[SDL_SCANCODE_LEFT] ^ key[SDL_SCANCODE_RIGHT]) {
		if (key[SDL_SCANCODE_LEFT]) d.x = -1;
		if (key[SDL_SCANCODE_RIGHT]) d.x = 1;
	}

	if (key[SDL_SCANCODE_UP] ^ key[SDL_SCANCODE_DOWN]) {
		if (key[SDL_SCANCODE_UP]) d.y = -1;
		if (key[SDL_SCANCODE_DOWN]) d.y = 1;
	}

	a->phys.vel.x = a->speed.x * d.x;
	a->phys.vel.y = a->speed.y * d.y;
}

void haz_actorCollide(haz_engine *e, haz_actor *a) {
	haz_line _a = {
		a->rend.x,
		a->rend.x + a->rend.w,
		a->rend.y,
		a->rend.y + a->rend.h
	};

	a->col.l= false;
	a->col.r= false;
	for (int i = _a.y1; i < _a.y2; i++) {
		SDL_FPoint pl = {_a.x1 - 1, i};
		SDL_FPoint pr = {_a.x2, i};

		if (haz_pointInTile(e, pl)) { a->col.l= true; }
		if (haz_pointInTile(e, pr)) { a->col.r= true; }
	}

	a->col.t = false;
	a->col.b = false;
	for (int i = _a.x1; i < _a.x2; i++) {
		SDL_FPoint pt = {i, _a.y1 - 1};
		SDL_FPoint pb = {i, _a.y2};

		if (haz_pointInTile(e, pt)) { a->col.t = true; }
		if (haz_pointInTile(e, pb)) { a->col.b = true; }
	}
}

bool haz_actorCollideArea(haz_actor *a, SDL_FRect r) {
	haz_line _a = {
		a->rend.x,
		a->rend.x + a->rend.w,
		a->rend.y,
		a->rend.y + a->rend.h
	};

	haz_line _r = {r.x, r.y, r.x + r.w, r.y + r.h};

	if (_a.x2 < _r.x1 || _a.x1 >= _r.x2) return false;
	if (_a.y2 < _r.y1 || _a.y1 >= _r.y2) return false;

	return true;
}
