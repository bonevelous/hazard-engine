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
#include "hazard-ui.h"

void haz_updateElement(haz_engine *e, haz_UIelement ui) {
	SDL_RenderTexture(e->ren, ui.tex, NULL, &ui.rend);
}

void haz_renderText(haz_engine *e, haz_font *font, haz_text *msg) {
	SDL_Color col = {0, 0, 0, 0xff};
	SDL_GetRenderDrawColor(e->ren, &col.r, &col.g, &col.b, NULL);

	SDL_SetTextureColorMod(font->tex, col.r, col.g, col.b);

	SDL_FRect src, dst;
	src.w = font->char_size.x;
	src.h = font->char_size.y;

	SDL_Point size = {
		font->bmp_size.x / font->char_size.x,
		font->bmp_size.y / font->char_size.y
	};

	for (int i = 0; i < strlen(msg->text); i++) {
		src.x = (msg->text[i] % size.x) * src.w;
		src.y = (msg->text[i] / size.x) * src.h;

		dst.x = (i * (src.w * msg->scale)) + msg->pos.x;
		dst.y = msg->pos.y;
		dst.w = src.w * msg->scale;
		dst.h = src.h * msg->scale;

		SDL_RenderTexture(e->ren, font->tex, &src, &dst);
	}
}
