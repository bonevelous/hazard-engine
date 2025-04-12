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

SDL_Texture *target = NULL;
SDL_FRect targ_clip = {0, 0, 320, 200};

SDL_Color harvkey = {0xff, 0x00, 0xff, 0xff};
haz_actor Harvey;
haz_actor gdoor;

void haz_windowSetup(haz_engine *e) {
	e->title = "Hazard Engine";
	e->window_size.x = 640;
	e->window_size.y = 400;
}

bool haz_loadData(haz_engine *e) {
	Harvey.flag = ACTOR_PLAYER;
	Harvey.frame.w = 16;
	Harvey.frame.h = 32;
	Harvey.position.w = 16;
	Harvey.position.h = 32;
	Harvey.speed.x = 4;
	Harvey.speed.y = 4;

	gdoor.frame.w = 32;
	gdoor.frame.h = 48;
	gdoor.position.w = 32;
	gdoor.position.h = 48;

	target = SDL_CreateTexture(e->renderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, e->window_size.x, e->window_size.y);
	if (target == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return 1;
	}
	SDL_SetTextureScaleMode(target, SDL_SCALEMODE_NEAREST);

	if (!haz_loadMap(e, "data/levels/E1L1.dat")) {
		printf("ERROR: haz_loadMap() failed.\n");
		return false;
	}

	for (int i = 0; i < MAP_W * MAP_H; i++) {
		int x = i % MAP_W;
		int y = i / MAP_H;
		if (e->map[y][x] == 'H') {
			Harvey.position.x = x * e->tile_size.x;
			Harvey.position.y = (y * e->tile_size.y) - 16;
		}
		if (e->map[y][x] == 'G') {
			gdoor.position.x = x * e->tile_size.x;
			gdoor.position.y = (y * e->tile_size.y) - 32;
		}
	}

	haz_loadTexture(e, &Harvey.sprite, &harvkey, "data/art/harvey.bmp");
	if (Harvey.sprite == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return false;
	}

	haz_loadTexture(e, &gdoor.sprite, &harvkey, "data/art/green_door.bmp");
	if (gdoor.sprite == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

void haz_pollKeyboard(haz_engine *e, SDL_Keycode sym) {
	switch(sym) {
		case SDLK_ESCAPE:
			e->status = 0;
			break;
		default:
			break;
	}
}

void haz_pollEvent(haz_engine *e) {
	while(SDL_PollEvent(&e->event)) {
		switch(e->event.type) {
			case SDL_EVENT_QUIT:
				e->status = 0;
				break;
			case SDL_EVENT_KEY_DOWN:
				haz_pollKeyboard(e, e->event.key.key);
				break;
			default:
				break;
		}
	}
}

void haz_app(haz_engine *e) {
	SDL_SetRenderTarget(e->renderer, target);
	SDL_SetRenderDrawColor(e->renderer, e->clear_color.r, e->clear_color.g,
		e->clear_color.b, 0xff);
	SDL_RenderFillRect(e->renderer, NULL);

	haz_renderBackground(e);

	SDL_RenderTexture(e->renderer, gdoor.sprite, &gdoor.frame,
		&gdoor.position);

	haz_actorUpdate(e, &Harvey);
	haz_actorCollideRect(&Harvey, gdoor.position);

	SDL_SetRenderTarget(e->renderer, NULL);
	SDL_RenderTexture(e->renderer, target, &targ_clip, NULL);
}

void haz_renderUI(haz_engine *e) { /* pass */ }

void haz_freeData(haz_engine *e) {
	SDL_DestroyTexture(gdoor.sprite);
	gdoor.sprite = NULL;

	SDL_DestroyTexture(Harvey.sprite);
	Harvey.sprite = NULL;

	SDL_DestroyTexture(target);
	target = NULL;
}
