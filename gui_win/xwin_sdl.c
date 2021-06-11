/*
 * File name: xwin_sdl.c
 * Date:      2015/06/18 14:37
 * Author:    Jan Faigl && Jakub Sukdol
 */

#include <assert.h>
#include <stdbool.h>

#include <time.h>
#include "xwin_sdl.h"
#include "../utils/utils.h"
#include "icon.h"
#include "button.h"
#include "button_anim.h"

static SDL_Window *win = NULL;

#define BUTTON_SECTION_COLOR 177

#define BUTTON_SEC_Y 344

bool animation = false;

int xwin_init(int w, int h)
{
   int r;
   r = SDL_Init(SDL_INIT_VIDEO);
   assert(win == NULL);
   win = SDL_CreateWindow("PRG Semester Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w + BUTTON_WIN_W, h,  SDL_WINDOW_RESIZABLE);
   assert(win != NULL);
   SDL_SetWindowTitle(win, "FRACTAL OBSERVER");
   SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(icon, 64, 48, 24, 64 * 3, 0xff, 0xff00, 0xff0000, 0x0000);
   SDL_SetWindowIcon(win, surface);


   SDL_UpdateWindowSurface(win);

   SDL_FreeSurface(surface);
   return r;
}

void xwin_close()
{
   assert(win != NULL);
   SDL_DestroyWindow(win);
   SDL_Quit();
}

void xwin_redraw(int w, int h, unsigned char *img)
{
   SDL_Surface *scr = SDL_GetWindowSurface(win);
   for(int y = 0; y < scr->h; ++y) {
      for(int x = 0; x < scr->w - BUTTON_WIN_W; ++x) {
         const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
         Uint8 *px = (Uint8*)scr->pixels + idx;
         *(px + scr->format->Rshift / 8) = *(img++);
         *(px + scr->format->Gshift / 8) = *(img++);
         *(px + scr->format->Bshift / 8) = *(img++);
      }
   }
   int i = 0;
   if (!animation) {
      for(int y = 0; y < scr->h; ++y) {
         for(int x = scr->w - BUTTON_WIN_W; x < scr->w; ++x) {
            const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
            Uint8 *px = (Uint8*)scr->pixels + idx;
            if (y < BUTTON_SEC_Y) {
               *(px + scr->format->Rshift / 8) = butt[i++];
               *(px + scr->format->Gshift / 8) = butt[i++];
               *(px + scr->format->Bshift / 8) = butt[i++];
            } else {
               *(px + scr->format->Rshift / 8) = 255;
               *(px + scr->format->Gshift / 8) = 255;
               *(px + scr->format->Bshift / 8) = 255;
            }
         }
      }
   } else {
      for(int y = 0; y < scr->h; ++y) {
         for(int x = scr->w - BUTTON_WIN_W; x < scr->w; ++x) {
            const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
            Uint8 *px = (Uint8*)scr->pixels + idx;
            if (y < BUTTON_SEC_Y) {
               *(px + scr->format->Rshift / 8) = button_anim[i++];
               *(px + scr->format->Gshift / 8) = button_anim[i++];
               *(px + scr->format->Bshift / 8) = button_anim[i++];
            } else {
               *(px + scr->format->Rshift / 8) = 255;
               *(px + scr->format->Gshift / 8) = 255;
               *(px + scr->format->Bshift / 8) = 255;
            }
         }
      }
   }
   SDL_UpdateWindowSurface(win);
}

void xwin_poll_events(void) 
{
   SDL_Event event;
   while (SDL_PollEvent(&event));
}

void xwin_resize(int width, int height) 
{
   SDL_SetWindowSize(win, width + BUTTON_WIN_W, height);
}

void save_image(char *scr_name)
{
   IMG_Init(IMG_INIT_PNG);
   SDL_Surface *scr = SDL_GetWindowSurface(win);
   IMG_SavePNG(scr, scr_name);
   SDL_FreeSurface(scr);
   IMG_Quit();
}

void xwin_redraw_button(int button, bool anim, bool anim_change) {
   animation = anim;
   SDL_Surface *scr = SDL_GetWindowSurface(win);
   int i = 0;
   switch (button) {
      case REDRAW_ANIM:
         if (anim) {
            for(int y = 0; y < scr->h; ++y) {
               for(int x = scr->w - BUTTON_WIN_W; x < scr->w; ++x) {
                  const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
                  Uint8 *px = (Uint8*)scr->pixels + idx;
                  if (y < BUTTON_SEC_Y) {
                     *(px + scr->format->Rshift / 8) = button_anim[i++];
                     *(px + scr->format->Gshift / 8) = button_anim[i++];
                     *(px + scr->format->Bshift / 8) = button_anim[i++];
                  } else {
                     break;
                  }
               }
            }
         } else {
            int ind = 0;
            for(int y = 0; y < scr->h; ++y) {
               for(int x = scr->w - BUTTON_WIN_W; x < scr->w; ++x) {
                  const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
                  Uint8 *px = (Uint8*)scr->pixels + idx;
                  if (y < BUTTON_SEC_Y) {
                     *(px + scr->format->Rshift / 8) = butt[ind++];
                     *(px + scr->format->Gshift / 8) = butt[ind++];
                     *(px + scr->format->Bshift / 8) = butt[ind++];
                  } else {
                     *(px + scr->format->Rshift / 8) = 255;
                     *(px + scr->format->Gshift / 8) = 255;
                     *(px + scr->format->Bshift / 8) = 255;
                  }
               }
            }
         }
         break;
      case REDRAW_C_IM_M:
         break;
      case REDRAW_C_IM_P:
         break;
      case REDRAW_C_RE_M:
         break;
      case REDRAW_C_RE_P:
         break;
   }
   SDL_UpdateWindowSurface(win);
   if (!anim_change) {
      SDL_Delay(500);

      int ind = 0;
      for(int y = 0; y < scr->h; ++y) {
         for(int x = scr->w - BUTTON_WIN_W; x < scr->w; ++x) {
            const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
            Uint8 *px = (Uint8*)scr->pixels + idx;
            if (y < BUTTON_SEC_Y) {
               *(px + scr->format->Rshift / 8) = butt[ind++];
               *(px + scr->format->Gshift / 8) = butt[ind++];
               *(px + scr->format->Bshift / 8) = butt[ind++];
            } else {
               *(px + scr->format->Rshift / 8) = 255;
               *(px + scr->format->Gshift / 8) = 255;
               *(px + scr->format->Bshift / 8) = 255;
            }
         }
      }
      SDL_UpdateWindowSurface(win);

   }
}


/* end of xwin_sdl.c */
