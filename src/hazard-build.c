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
#include "hazard-ui.h"
#include "hazard-build.h"

#define MAX_Z 8
#define MIN_Z 0.125

haz_UIelement map_ui;
haz_UIelement frame = {{32, 24, 800, 600}};

build_mode mode = TILE;
haz_UIelement smode = {{8, 56, 16, 16}};
SDL_FRect tmode = {8, 32, 16, 16};

haz_font mfont;

bool move_view = false;
SDL_Point last_press = {0, 0};
SDL_Point last_pos = {0, 0};
double zoom = 1;
SDL_PixelFormat pix = SDL_PIXELFORMAT_RGBA8888;
SDL_TextureAccess acc = SDL_TEXTUREACCESS_TARGET;

haz_text save_text = {
	{4, 4},
	1,
	"SAVE"
};

void haz_windowSetup(haz_engine *e) {
	e->size.x = 800;
	e->size.y = 600;
	e->window_flag = SDL_WINDOW_RESIZABLE;
}

bool haz_loadData(haz_engine *e) {
	map_ui.rend.w = MAP_W * e->tilesize.x;
	map_ui.rend.h = MAP_H * e->tilesize.y;

	map_ui.tex = SDL_CreateTexture(e->ren, pix, acc, map_ui.rend.w,
		map_ui.rend.h);

	if (map_ui.tex == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return false;
	}

	SDL_Color k = {0, 0, 0, 0xff};
	haz_loadTexture(e, &mfont.tex, &k, "data/art/mainfont.bmp");
	if (mfont.tex == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return false;
	}

	SDL_GetTextureSize(mfont.tex, &mfont.bmp_size.x,
		&mfont.bmp_size.y);

	mfont.char_size.x = 9;
	mfont.char_size.y = 16;

	for (int i = 0; i < MAP_W * MAP_H; i++) {
		int x = i % MAP_W;
		int y = i / MAP_W;

		e->map.tiles[y][x].active = false;
	}

	k.r = 0xff;
	k.b = 0xff;
	haz_loadTexture(e, &smode.tex, &k, "data/art/spritemode.bmp");
	if (smode.tex == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

bool hbuild_saveMap(haz_engine *e, const char *filename) {
	FILE *file = NULL;
	file = fopen(filename, "wb");
	if (file == NULL) {
		printf("ERROR: Failed fopen() with %s\n", filename);
		return false;
	}

	size_t size = sizeof(e->map);
	size_t w = fwrite(&e->map, sizeof(haz_map), size, file);

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
				if (e->event.wheel.y > 0 && zoom < MAX_Z) {
					zoom *= 2;
				}
				if (e->event.wheel.y < 0 && zoom > MIN_Z) {
					zoom /= 2;
				}
				break;
			default:
				break;
		}
	}
}

void haz_app(haz_engine *e) {
	int wx, wy;
	SDL_GetWindowSize(e->window, &wx, &wy);

	frame.rend.w = wx - frame.rend.x - 6;
	frame.rend.h = wy - frame.rend.y - 6;

	frame.tex = SDL_CreateTexture(e->ren, pix, acc, frame.rend.w,
		frame.rend.h);

	map_ui.rend.x = e->camera.x;
	map_ui.rend.y = e->camera.y;
	map_ui.rend.w = (MAP_W * e->tilesize.x) * zoom;
	map_ui.rend.h = (MAP_H * e->tilesize.y) * zoom;

	SDL_SetRenderDrawColor(e->ren, 0xc0, 0xc0, 0xc0, 0xff);
	SDL_RenderFillRect(e->ren, NULL);

	SDL_FlushRenderer(e->ren);

	SDL_SetRenderTarget(e->ren, frame.tex);
	SDL_SetRenderDrawColor(e->ren, 0x80, 0x80, 0x80, 0xff);
	SDL_RenderFillRect(e->ren, NULL);

	SDL_FlushRenderer(e->ren);

	SDL_SetRenderTarget(e->ren, map_ui.tex);
	SDL_SetRenderDrawColor(e->ren, 0, 0, 0, 0xff);
	SDL_RenderFillRect(e->ren, NULL);

	haz_renderBackground(e, false);

	SDL_FRect tile = {0, 0, e->tilesize.x, e->tilesize.y};
	SDL_FRect pos = {0, 0, e->tilesize.x, e->tilesize.y};

	int x = ((e->mouse.pos.x - e->camera.x - frame.rend.x) / zoom) /
		e->tilesize.x;

	int y = ((e->mouse.pos.y - e->camera.y - frame.rend.y) / zoom) /
		e->tilesize.y;

	pos.x = ((float) x) * e->tilesize.x;
	pos.y = ((float) y) * e->tilesize.y;

	SDL_FRect mrend = {0, 0, map_ui.rend.w, map_ui.rend.h};
	mrend.x = map_ui.rend.x + frame.rend.x;
	mrend.y = map_ui.rend.y + frame.rend.y;
	if (haz_pointInRect(e->mouse.pos, mrend)) {
		SDL_HideCursor();
	}
	else { SDL_ShowCursor(); };

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

	if (x >= 0 && x < MAP_W && y >= 0 && y < MAP_H &&
		haz_pointInRect(e->mouse.pos, mrend)) {

		SDL_RenderTexture(e->ren, e->tileset, &tile, &pos);

		if (e->mouse.state == SDL_BUTTON_MASK(1)) {
			e->map.tiles[y][x].active = true;
		}

		if (e->mouse.state == SDL_BUTTON_MASK(3)) {
			e->map.tiles[y][x].active = false;
		}
	}

	SDL_SetRenderTarget(e->ren, frame.tex);
	SDL_RenderTexture(e->ren, map_ui.tex, NULL, &map_ui.rend);
	SDL_FlushRenderer(e->ren);
	SDL_SetRenderTarget(e->ren, NULL);
	SDL_RenderTexture(e->ren, frame.tex, NULL, &frame.rend);

	SDL_DestroyTexture(frame.tex);
	frame.tex = NULL;

	SDL_RenderTexture(e->ren, e->tileset, &tile, &tmode);
	SDL_SetRenderDrawColor(e->ren, 0xff, 0, 0, 0xff);
	if (haz_pointInRect(e->mouse.pos, tmode)) {
		SDL_RenderRect(e->ren, &tmode);
		if (e->mouse.state == SDL_BUTTON_MASK(1)) { mode = TILE; }
	}

	if (haz_pointInRect(e->mouse.pos, smode.rend)) {
		SDL_RenderRect(e->ren, &smode.rend);
		if (e->mouse.state == SDL_BUTTON_MASK(1)) { mode = SPRITE; }
	}

	switch(mode) {
		/*case TILE:
			tempchar = '#';
			break;
		case SPRITE:
			tempchar = 'H';
			break;*/
		default:
			break;
	}

	SDL_RenderTexture(e->ren, smode.tex, NULL, &smode.rend);

	SDL_SetRenderDrawColor(e->ren, 0, 0, 0, 0xff);
	haz_renderText(e, &mfont, &save_text);
}

void haz_freeData(haz_engine *e) {
	SDL_DestroyTexture(smode.tex);
	smode.tex = NULL;

	SDL_DestroyTexture(frame.tex);
	frame.tex = NULL;

	SDL_DestroyTexture(mfont.tex);
	mfont.tex = NULL;

	SDL_DestroyTexture(map_ui.tex);
	map_ui.tex = NULL;
}
