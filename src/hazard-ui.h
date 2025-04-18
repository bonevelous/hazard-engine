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

#ifndef HAZARD_UI_H
#define HAZARD_UI_H

#include "hazard-engine.h"

typedef struct haz_UIelement {
	SDL_FRect rend;
	SDL_FRect clip;
	SDL_Texture *tex;
} haz_UIelement;

typedef struct haz_font {
	SDL_Texture *tex;
	SDL_FPoint bmp_size;
	SDL_Point char_size;
} haz_font;

typedef struct haz_text {
	SDL_Point pos;
	double scale;
	char *text;
} haz_text;

void haz_renderElement(haz_engine *e, haz_UIelement ui);
void haz_renderText(haz_engine *e, haz_font *font, haz_text *msg);

#endif //HAZARD_UI_H
