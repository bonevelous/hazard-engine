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
#include "hazard-build.h"

void haz_windowSetup(haz_engine *e) {
	e->title = "Hazard Engine";
	e->window_size.x = 800;
	e->window_size.y = 600;
	e->window_flag = SDL_WINDOW_RESIZABLE;
}

bool haz_loadData(haz_engine *e) {
	e->camera.y = 24;
	e->clear_color.r = 0x55;
	e->clear_color.g = 0x55;
	e->clear_color.b = 0x55;

	for (int i = 0; i < MAP_W * MAP_H; i++) {
		int x = i % MAP_W;
		int y = i / MAP_W;

		e->map[y][x] = '.';
	}

	return true;
}

bool hbuild_saveMap(haz_engine *e, const char *filename) {
	FILE *file = NULL;
	file = fopen(filename, "w");
	if (file == NULL) {
		printf("ERROR: Failed fopen() with %s\n", filename);
		return false;
	}

	for (int i = 0; i < MAP_W * MAP_H; i++) {
		int x = i % MAP_W;
		int y = i / MAP_W;

		fputc(e->map[y][x], file);
		if (x == MAP_W - 1) fputc('\n', file);
	}
	fputc('\0', file);

	fclose(file);
	file = NULL;

	return true;
}

void haz_pollKeyboard(haz_engine *e, SDL_Keycode sym) {
	switch(sym) {
		case SDLK_S:
			if (!hbuild_saveMap(e, "data/levels/E1L1.dat")) {
				printf("Could not save level.\n");
			}
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
	SDL_SetRenderDrawColor(e->renderer, 0, 0, 0, 0xff);
	SDL_FRect mapRect = {
		e->camera.x,
		e->camera.y,
		MAP_W * e->tile_size.x,
		MAP_H * e->tile_size.y
	};
	SDL_RenderFillRect(e->renderer, &mapRect);

	haz_renderBackground(e);

	SDL_FRect tile = {0, 0, e->tile_size.x, e->tile_size.y};
	SDL_FRect pos = {0, 0, e->tile_size.x, e->tile_size.y};
	pos.x = (((int) e->mouse.position.x / e->tile_size.x) * e->tile_size.x)
		+ e->camera.x;

	pos.y = (((int) e->mouse.position.y / e->tile_size.y) * e->tile_size.y)
		+ e->camera.y;

	SDL_RenderTexture(e->renderer, e->tileset, &tile, &pos);

	int x = (pos.x - e->camera.x) / e->tile_size.x;
	int y = (pos.y - e->camera.y) / e->tile_size.y;

	if (e->mouse.state == SDL_BUTTON_MASK(1) && x >= 0 && x < MAP_W
		&& y >= 0 && y < MAP_H) {

		/*e->map[y][x].active = true;
		e->map[y][x].tile.x = 0;
		e->map[y][x].tile.y = 0;*/

		e->map[y][x] = '#';
	}

	if (e->mouse.state == SDL_BUTTON_MASK(3) && x >= 0 && x < MAP_W
		&& y >= 0 && y < MAP_H) {

		//e->map[y][x].active = false;

		e->map[y][x] = '.';
	}
}

void haz_renderUI(haz_engine *e) {
	/*int w, h;
	SDL_GetWindowSize(e->window, &w, &h);

	SDL_FRect topbar = {0, 0, w, 24};
	SDL_SetRenderDrawColor(e->renderer, 0xc0, 0xc0, 0xc0, 0xff);
	SDL_RenderFillRect(e->renderer, &topbar);*/

	haz_renderText(e, NULL, 4, 4, "SAVE");
}

void haz_freeData(haz_engine *e) { /* pass */ }
