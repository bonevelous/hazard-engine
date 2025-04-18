/*   Hazard Engine - 2D Game engine in C
 *   Copyright (C) 2025 Matthew Gobbi
 *   hazard-engine.h - Engine Header
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

#ifndef HAZARD_ENGINE_H
#define HAZARD_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <SDL3/SDL.h>

#define MAP_W 16
#define MAP_H 12
#define MAX_OBJ 256

typedef struct haz_line {
	int x1;
	int x2;
	int y1;
	int y2;
} haz_line;

typedef struct haz_mouse {
	SDL_MouseButtonFlags state;
	SDL_FPoint pos;
} haz_mouse;

typedef struct haz_tile {
	bool active;
	SDL_Point tile;
} haz_tile;

typedef struct haz_objectInfo {
	bool lonely;
	SDL_Point pos;
	SDL_Texture *tex;
} haz_objectInfo;

typedef struct haz_map {
	haz_tile tiles[MAP_H][MAP_W];
	haz_objectInfo *objects[MAX_OBJ];
} haz_map;

typedef struct haz_engine {
	int status;
	char *progname;
	char *version;
	SDL_Window *window;
	char *title;
	SDL_Rect size;
	int window_flag;
	SDL_Renderer *ren;
	SDL_Texture *tileset;
	SDL_Point tilesize;
	SDL_Color clear_color;
	SDL_Point camera;
	SDL_Event event;
	haz_mouse mouse;
	haz_map map;
} haz_engine;

void haz_printHelp(void);
void haz_printVersion(haz_engine e);

void haz_windowSetup(haz_engine *e);
bool haz_init(haz_engine *e);
bool haz_loadData(haz_engine *e);
bool haz_loadMap(haz_engine *e, const char *filename);
bool haz_loadTexture(haz_engine *e, SDL_Texture **t, SDL_Color *key,
	const char *filename);

void haz_process(haz_engine *e, int delay);
void haz_pollEvent(haz_engine *e);
void haz_pollKeyboard(haz_engine *e, SDL_Keycode sym);
void haz_app(haz_engine *e);

bool haz_pointInRect(SDL_FPoint p, SDL_FRect r);
bool haz_pointInTile(haz_engine *e, SDL_FPoint p);

void haz_renderBackground(haz_engine *e, bool camera);
//void haz_renderForeground(haz_engine *e);

void haz_freeData(haz_engine *e);
void haz_quit(haz_engine *e);

#endif //HAZARD_ENGINE_H
