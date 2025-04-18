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

void haz_printHelp(void) {
	printf("-h  |  --help       Print this help message.\n"
		"-v  |  --version    Print version and copyright.\n"
	);
}

void haz_printVersion(haz_engine e) {
	printf("%s version %s\n", e.progname, e.version);
}

bool haz_init(haz_engine *e) {
	e->status = 1;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		printf("ERROR: %s\n", SDL_GetError());
		return 1;
	}

	if (!SDL_CreateWindowAndRenderer(e->title, e->size.x, e->size.y,
		e->window_flag, &e->window, &e->ren)) {

		printf("ERROR: %s\n", SDL_GetError());
		return 1;
	}

	SDL_SetRenderDrawBlendMode(e->ren, SDL_BLENDMODE_BLEND);

	haz_loadTexture(e, &e->tileset, NULL, "data/art/tiles.bmp");
	if (e->tileset == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return false;
	}
	e->tilesize.x = 16;
	e->tilesize.y = 16;

	SDL_SetTextureScaleMode(e->tileset, SDL_SCALEMODE_NEAREST);

	if (!haz_loadData(e)) {
		printf("ERROR: haz_loadData() failed.\n");
	}

	return true;
}

bool haz_loadTexture(haz_engine *e, SDL_Texture **texture, SDL_Color *key,
	const char *filename) {

	SDL_Surface *surface = NULL;
	surface = SDL_LoadBMP(filename);
	if (surface == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return false;
	}

	if (key != NULL) {
		uint32_t color = SDL_MapSurfaceRGB(surface, key->r, key->g,
			key->b);
		SDL_SetSurfaceColorKey(surface, true, color);
	}

	*texture = SDL_CreateTextureFromSurface(e->ren, surface);
	if (*texture == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return false;
	}

	SDL_DestroySurface(surface);
	surface = NULL;

	return true;
}

void haz_renderBackground(haz_engine *e, bool camera) {
	for (int i = 0; i < MAP_W * MAP_H; i++) {
		SDL_FRect tileSRC = {0, 0, e->tilesize.x, e->tilesize.y};
		SDL_FRect tileDST = {0, 0, e->tilesize.x, e->tilesize.y};
		int x = i % MAP_W;
		int y = i / MAP_W;

		if (camera) {
			tileDST.x = (tileSRC.w * x) + e->camera.x;
			tileDST.y = (tileSRC.h * y) + e->camera.y;
		}
		else {
			tileDST.x = tileSRC.w * x;
			tileDST.y = tileSRC.h * y;
		}

		tileSRC.x = tileSRC.w * e->map.tiles[y][x].tile.x;
		tileSRC.y = tileSRC.h * e->map.tiles[y][x].tile.y;
		if (e->map.tiles[y][x].active == true) {
			SDL_RenderTexture(e->ren, e->tileset, &tileSRC,
				&tileDST);
		}
	}
}

bool haz_loadMap(haz_engine *e, const char *filename) {
	FILE *file = NULL;
	file = fopen(filename, "rb");
	if (file == NULL) {
		printf("ERROR: Failed fopen() with %s\n", filename);
		return false;
	}

	size_t size = sizeof(e->map);
	size_t r = fread(&e->map, sizeof(haz_map), size, file);

	fclose(file);
	file = NULL;

	return true;
}

bool haz_pointInRect(SDL_FPoint p, SDL_FRect r) {
	if ((int) p.x < (int) r.x) return false;
	if ((int) p.y < (int) r.y) return false;
	if ((int) p.x >= (int) r.x + (int) r.w) return false;
	if ((int) p.y >= (int) r.y + (int) r.h) return false;
	return true;
}

bool haz_pointInTile(haz_engine *e, SDL_FPoint p) {
	int x = ((int) p.x / e->tilesize.x);
	int y = ((int) p.y / e->tilesize.y);
	if (x > -1 && x < MAP_W && y > -1 && y < MAP_H &&
		e->map.tiles[y][x].active) { return true; }
	else { return false; }
}

void haz_process(haz_engine *e, int delay) {
	SDL_Delay(delay);

	e->mouse.state = SDL_GetMouseState(&e->mouse.pos.x, &e->mouse.pos.y);

	haz_pollEvent(e);

	SDL_SetRenderDrawColor(e->ren, 0, 0, 0, 0xff);
	SDL_RenderClear(e->ren);

	SDL_SetRenderDrawColor(e->ren, e->clear_color.r, e->clear_color.g,
		e->clear_color.b, 0xff);
	SDL_RenderFillRect(e->ren, NULL);

	haz_app(e);

	SDL_RenderPresent(e->ren);
}

void haz_quit(haz_engine *e) {
	haz_freeData(e);

	SDL_DestroyTexture(e->tileset);
	e->tileset = NULL;

	SDL_DestroyRenderer(e->ren);
	e->ren = NULL;

	SDL_DestroyWindow(e->window);
	e->window = NULL;

	SDL_Quit();
}
