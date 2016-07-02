#include "hal.h"
#include "device/video.h"
#include "device/palette.h"

#include <string.h>
#include <stdlib.h>

int get_fps();

void blit_helper(SDL_Surface *src,int16_t src_x, int16_t src_y, SDL_Surface *dst,int16_t dst_x, int16_t dst_y, uint16_t clp_width, uint16_t clp_height) {
	assert(src_x >= 0 && src_y >= 0 && src_x + clp_width <= src->w && src_y + clp_height <= src->h);
	assert(dst_x >= 0 && dst_y >= 0 && dst_x + clp_width <= dst->w && dst_y + clp_height <= dst->h);
	/*Log("BytesPerPixel: %d\n", src->format->BytesPerPixel);*/
	/*if(src->format->BytesPerPixel == 1) {*/
		int16_t x1 = src_x, y1 = src_y;
		int16_t x2 = dst_x, y2 = dst_y;
		uint16_t m = clp_width, n = clp_height;
		uint16_t width1 = src->w, width2 = dst->w;

		int i,j;
		uint8_t *src_pixel = (uint8_t *)(src->pixels) + y1 * width1 + x1;
		uint8_t *dst_pixel = (uint8_t *)(dst->pixels) + y2 * width2 + x2;
		for(i = 0; i < n; ++i) {
			for(j = 0; j < m; ++j) {
				*dst_pixel++ = *src_pixel++;
			}
			src_pixel += width1 - m;
			dst_pixel += width2 - m;
		}
	/*}*/
	/*else {*/
		/*assert(0);*/
	/*}*/
}

void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, 
		SDL_Surface *dst, SDL_Rect *dstrect) {
	assert(dst && src);

	/* TODO: Performs a fast blit from the source surface to the 
	 * destination surface. Only the position is used in the
	 * ``dstrect'' (the width and height are ignored). If either
	 * ``srcrect'' or ``dstrect'' are NULL, the entire surface 
	 * (``src'' or ``dst'') is copied. The final blit rectangle 
	 * is saved in ``dstrect'' after all clipping is performed
	 * (``srcrect'' is not modified).
	 */
	if(srcrect == NULL && dstrect == NULL) {
		blit_helper(src, 0, 0, dst, 0, 0, src->w, src->h);
		/*the entire surface is copied*/
	}
	else if(srcrect != NULL && dstrect != NULL) {
		/*copy according to srcrect and dstrect*/
		blit_helper(src, srcrect->x, srcrect->y, dst, dstrect->x, dstrect->y, srcrect->w, srcrect->h);
		dstrect->w = srcrect->w;
		dstrect->h = srcrect->h;
	}
	else if(srcrect != NULL && dstrect == NULL) {
		blit_helper(src, srcrect->w, srcrect->h, dst, 0, 0, srcrect->w, srcrect->h);
	}
	else {
		blit_helper(src, 0, 0, dst, dstrect->x, dstrect->y, src->w, src->h);
		dstrect->w = src->w;
		dstrect->h = src->h;
	}

}

extern SDL_Surface *gpScreen;

void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color) {
	assert(dst);
	assert(color <= 0xff);
	assert(dst->format->BytesPerPixel == 1);
	/* TODO: Fill the rectangle area described by ``dstrect''
	 * in surface ``dst'' with color ``color''. If dstrect is
	 * NULL, fill the whole surface.
	 */
	int16_t x, y, w, h;
	if(dstrect == NULL) {
		x = y = 0;
		w = dst->w;
		h = dst->h;
	}
	else {
		x = dstrect->x;
		y = dstrect->y;
		w = dstrect->w;
		h = dstrect->h;
	}

	uint16_t width = dst->w;
	uint8_t *dst_pixel = (uint8_t *)(dst->pixels) + y * width + x;
	int i, j;
	for(i = 0; i < h; ++i) {
		for(j = 0; j < w; ++j) {
			*dst_pixel++ = color;
		}
		dst_pixel = dst_pixel - w + width;
	}
}

void SDL_UpdateRect(SDL_Surface *screen, int x, int y, int w, int h) {
	assert(screen);
	assert(screen->pitch == 320);
	if(screen->flags & SDL_HWSURFACE) {
		if(x == 0 && y == 0) {
			/* Draw FPS */
			vmem = VMEM_ADDR;
			char buf[80];
			sprintf(buf, "%dFPS", get_fps());
			draw_string(buf, 0, 0, 10);
		}
		return;
	}

	/* TODO: Copy the pixels in the rectangle area to the screen. */
	SDL_Rect rect;
	rect.x = x; rect.y = y; rect.w = w; rect.h = h;
	SDL_BlitSurface(screen, &rect, gpScreen, &rect);
}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, 
		int firstcolor, int ncolors) {
	assert(s);
	assert(s->format);
	assert(s->format->palette);
	assert(firstcolor == 0);

	if(s->format->palette->colors == NULL || s->format->palette->ncolors != ncolors) {
		if(s->format->palette->ncolors != ncolors && s->format->palette->colors != NULL) {
			/* If the size of the new palette is different 
			 * from the old one, free the old one.
			 */
			free(s->format->palette->colors);
		}

		/* Get new memory space to store the new palette. */
		s->format->palette->colors = malloc(sizeof(SDL_Color) * ncolors);
		assert(s->format->palette->colors);
	}

	/* Set the new palette. */
	s->format->palette->ncolors = ncolors;
	memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

	if(s->flags & SDL_HWSURFACE) {
		/* TODO: Set the VGA palette by calling write_palette(). */
		write_palette(colors, ncolors);
	}
}

/* ======== The following functions are already implemented. ======== */

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *srcrect, 
		SDL_Surface *dst, SDL_Rect *dstrect) {
	assert(src && dst);
	int x = (srcrect == NULL ? 0 : srcrect->x);
	int y = (srcrect == NULL ? 0 : srcrect->y);
	int w = (srcrect == NULL ? src->w : srcrect->w);
	int h = (srcrect == NULL ? src->h : srcrect->h);

	assert(dstrect);
	if(w == dstrect->w && h == dstrect->h) {
		/* The source rectangle and the destination rectangle
		 * are of the same size. If that is the case, there
		 * is no need to stretch, just copy. */
		SDL_Rect rect;
		rect.x = x;
		rect.y = y;
		rect.w = w;
		rect.h = h;
		SDL_BlitSurface(src, &rect, dst, dstrect);
	}
	else {
		/* No other case occurs in NEMU-PAL. */
		assert(0);
	}
}

SDL_Surface* SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
		uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask) {
	SDL_Surface *s = malloc(sizeof(SDL_Surface));
	assert(s);
	s->format = malloc(sizeof(SDL_PixelFormat));
	assert(s);
	s->format->palette = malloc(sizeof(SDL_Palette));
	assert(s->format->palette);
	s->format->palette->colors = NULL;

	s->format->BitsPerPixel = depth;

	s->flags = flags;
	s->w = width;
	s->h = height;
	s->pitch = (width * depth) >> 3;
	s->pixels = (flags & SDL_HWSURFACE ? (void *)VMEM_ADDR : malloc(s->pitch * height));
	assert(s->pixels);

	return s;
}

SDL_Surface* SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags) {
	return SDL_CreateRGBSurface(flags,  width, height, bpp,
			0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
}

void SDL_FreeSurface(SDL_Surface *s) {
	if(s != NULL) {
		if(s->format != NULL) {
			if(s->format->palette != NULL) {
				if(s->format->palette->colors != NULL) {
					free(s->format->palette->colors);
				}
				free(s->format->palette);
			}

			free(s->format);
		}
		
		if(s->pixels != NULL) {
			free(s->pixels);
		}

		free(s);
	}
}

