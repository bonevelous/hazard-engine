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

#define ZOOMS 8

bool move_view = false;
SDL_Point last_press = {0, 0};
SDL_Point last_pos = {0, 0};
int zoom_lev = 0;
double zoom[ZOOMS] = {0.125, 0.25, 0.5, 1, 1.5, 2, 3, 4};

void haz_windowSetup(haz_engine *e) {
	e->winsize.x = 800;
	e->winsize.y = 600;
	e->window_flag = SDL_WINDOW_RESIZABLE;
	zoom_lev = 3;
}

bool haz_loadData(haz_engine *e) {
	e->camera.y = 24;

	e->targclip.w = MAP_W * e->tilesize.x;
	e->targclip.h = MAP_H * e->tilesize.y;

	e->target = SDL_CreateTexture(e->renderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, e->targclip.w, e->targclip.h);

	if (e->target == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return false;
	}

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
			case SDL_EVENT_MOUSE_WHEEL:
				if (e->event.wheel.y > 0 &&
					zoom_lev < ZOOMS - 1) {

					zoom_lev += 1;
				}
				if (e->event.wheel.y < 0 && zoom_lev > 0) {
					zoom_lev -= 1;
				}
				break;
			default:
				break;
		}
	}
}

void haz_app(haz_engine *e) {
	e->targsize.x = e->camera.x;
	e->targsize.y = e->camera.y;
	e->targsize.w = (MAP_W * e->tilesize.x) * zoom[zoom_lev];
	e->targsize.h = (MAP_H * e->tilesize.y) * zoom[zoom_lev];

	SDL_SetRenderTarget(e->renderer, e->target);
	SDL_SetRenderDrawColor(e->renderer, e->clear_color.r, e->clear_color.g,
			e->clear_color.b, 0xff);

	SDL_SetRenderDrawColor(e->renderer, 0, 0, 0, 0xff);
	SDL_RenderFillRect(e->renderer, NULL);

	haz_renderBackground(e, false);

	SDL_FRect tile = {0, 0, e->tilesize.x, e->tilesize.y};
	SDL_FRect pos = {0, 0, e->tilesize.x, e->tilesize.y};

	int x = ((e->mouse.pos.x - e->camera.x) / zoom[zoom_lev]) /
		e->tilesize.x;

	int y = ((e->mouse.pos.y - e->camera.y) / zoom[zoom_lev]) /
		e->tilesize.y;

	pos.x = ((float) x) * e->tilesize.x;
	pos.y = ((float) y) * e->tilesize.y;

	/*if (haz_pointInRect(e->mouse.pos, e->targsize)) {
		SDL_HideCursor();
	}
	else { SDL_ShowCursor(); };*/

	if (e->mouse.state == SDL_BUTTON_MASK(2)) {
		if (!move_view) {
			last_press.x = e->mouse.pos.x;
			last_press.y = e->mouse.pos.y;
			last_pos.x = e->camera.x;
			last_pos.y = e->camera.y;
			move_view = true;
		}

		SDL_Point move_to = {
			e->mouse.pos.x - last_press.x,
			e->mouse.pos.y - last_press.y
		};

		e->camera.x = last_pos.x + move_to.x;
		e->camera.y = last_pos.y + move_to.y;
	}
	else { move_view = false; }

	if (x >= 0 && x < MAP_W && y >= 0 && y < MAP_H) {
		SDL_RenderTexture(e->renderer, e->tileset, &tile, &pos);

		/*e->map[y][x].active = true;
		  e->map[y][x].tile.x = 0;
		  e->map[y][x].tile.y = 0;*/

		if (e->mouse.state == SDL_BUTTON_MASK(1)) {
			e->map[y][x] = '#';
		}

		if (e->mouse.state == SDL_BUTTON_MASK(3)) {
			e->map[y][x] = '.';
		}
	}

	SDL_SetRenderTarget(e->renderer, NULL);
	SDL_RenderTexture(e->renderer, e->target, &e->targclip, &e->targsize);
}

void haz_renderUI(haz_engine *e) {
	/*int w, h;
	SDL_GetWindowSize(e->window, &w, &h);

	SDL_FRect topbar = {0, 0, w, 24};
	SDL_SetRenderDrawColor(e->renderer, 0xc0, 0xc0, 0xc0, 0xff);
	SDL_RenderFillRect(e->renderer, &topbar);*/

	haz_renderText(e, NULL, 4, 4, "SAVE");
}

void haz_freeData(haz_engine *e) {
	SDL_DestroyTexture(e->target);
	e->target = NULL;
}
