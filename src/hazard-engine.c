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

	if (!SDL_CreateWindowAndRenderer(e->title, e->winsize.x,
		e->winsize.y, e->window_flag, &e->window, &e->renderer)) {

		printf("ERROR: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Color mainkey = {0, 0, 0, 0xff};
	haz_loadTexture(e, &e->mainfont.font, &mainkey,
		"data/art/mainfont.bmp");
	if (e->mainfont.font == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return 1;
	}
	SDL_GetTextureSize(e->mainfont.font, &e->mainfont.texture_size.x,
		&e->mainfont.texture_size.y);
	e->mainfont.char_size.x = 9;
	e->mainfont.char_size.y = 16;

	SDL_SetRenderDrawBlendMode(e->renderer, SDL_BLENDMODE_BLEND);

	haz_loadTexture(e, &e->tileset, NULL, "data/art/tiles.bmp");
	if (e->tileset == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return false;
	}
	e->tilesize.x = 16;
	e->tilesize.y = 16;

	if (!haz_loadData(e)) {
		printf("ERROR: haz_loadData() failed.\n");
	}

	return true;
}

bool haz_loadTexture(haz_engine *e, SDL_Texture **texture, SDL_Color *key,
	const char *filename) {

	long size = pathconf(".", _PC_PATH_MAX);
	char *buf = NULL;
	char *cwd = NULL;
	buf = (char *) malloc((size_t) size);
	if (buf == NULL) {
		printf("ERROR: Couldn't get size of path\n");
		return false;
	}

	cwd = getcwd(buf, (size_t) size);
	if (cwd == NULL) {
		printf("ERROR: Couldn't get cwd\n");
		return false;
	}

	char *dir = NULL;
	dir = (char *) malloc(sizeof(buf) + 512);
	dir = strcpy(dir, "");
	dir = strcat(dir, cwd);
	dir = strcat(dir, "/");
	dir = strcat(dir, filename);
	if (dir == NULL) {
		printf("ERROR: Couldn't find file\n");
		return false;
	}

	SDL_Surface *surface = NULL;
	surface = SDL_LoadBMP(dir);
	if (surface == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return false;
	}

	if (key != NULL) {
		uint32_t color = SDL_MapSurfaceRGB(surface, key->r, key->g,
			key->b);
		SDL_SetSurfaceColorKey(surface, true, color);
	}

	*texture = SDL_CreateTextureFromSurface(e->renderer, surface);
	if (*texture == NULL) {
		printf("ERROR: %s\n", SDL_GetError());
		return false;
	}

	SDL_DestroySurface(surface);
	surface = NULL;

	free(dir);
	dir = NULL;

	free(cwd);
	cwd = NULL;

	return true;
}

bool haz_saveMap(haz_engine *e, const char *filename) {
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
		//tileSRC.x = tileSRC.w * e->map[y][x].tile.x;
		//tileSRC.y = tileSRC.h * e->map[y][x].tile.y;
		//if (e->map[y][x].active == true) {
		if (e->map[y][x] == '#') {
			SDL_RenderTexture(e->renderer, e->tileset, &tileSRC,
				&tileDST);
		}
	}
}

void haz_renderText(haz_engine *e, haz_font *font, int x, int y, char *text) {
	SDL_FRect textSRC = {0, 0, 9, 16};
	if (font != NULL) {
		textSRC.w = font->char_size.x;
		textSRC.h = font->char_size.y;
	}

	SDL_Point size = {
		e->mainfont.texture_size.x / 9,
		e->mainfont.texture_size.y / 16
	};

	if (font != NULL) {
		size.x = (font->texture_size.x / textSRC.w);
		size.y = (font->texture_size.y / textSRC.h);
	}

	for (int i = 0; i < strlen(text); i++) {
		textSRC.x = (text[i] % size.x) * textSRC.w;
		textSRC.y = (text[i] / size.x) * textSRC.h;

		SDL_FRect textDST = {
			(i * textSRC.w) + x,
			y, textSRC.w, textSRC.h
		};

		if (font == NULL) {
			SDL_RenderTexture(e->renderer, e->mainfont.font,
				&textSRC, &textDST);
		}
		else {
			SDL_RenderTexture(e->renderer, font->font, &textSRC,
				&textDST);
		}
	}
}

bool haz_loadMap(haz_engine *e, const char *filename) {
	FILE *file = NULL;
	file = fopen(filename, "r");
	if (file == NULL) {
		printf("ERROR: Failed fopen() with %s\n", filename);
		return false;
	}

	int x = 0;
	int y = 0;
	char c;
	while ((c = fgetc(file)) != EOF) {
		if (c == '\n') {
			x = 0;
			++y;
		}
		else {
			e->map[y][x] = c;
			++x;
		}
	}

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
		e->map[y][x] == '#') { return true; }
	else { return false; }
}

void haz_process(haz_engine *e, int delay) {
	SDL_Delay(delay);

	e->mouse.state = SDL_GetMouseState(&e->mouse.pos.x, &e->mouse.pos.y);

	haz_pollEvent(e);

	SDL_SetRenderDrawColor(e->renderer, 0, 0, 0, 0xff);
	SDL_RenderClear(e->renderer);

	SDL_SetRenderDrawColor(e->renderer, e->clear_color.r, e->clear_color.g,
		e->clear_color.b, 0xff);
	SDL_RenderFillRect(e->renderer, NULL);

	haz_app(e);
	//haz_renderForeground(e);
	haz_renderUI(e);

	SDL_RenderPresent(e->renderer);
}

void haz_quit(haz_engine *e) {
	haz_freeData(e);

	SDL_DestroyTexture(e->tileset);
	e->tileset = NULL;

	SDL_DestroyTexture(e->mainfont.font);
	e->mainfont.font = NULL;

	SDL_DestroyRenderer(e->renderer);
	e->renderer = NULL;

	SDL_DestroyWindow(e->window);
	e->window = NULL;

	SDL_Quit();
}
