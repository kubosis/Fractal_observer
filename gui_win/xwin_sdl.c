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
#include "textures/button_anim.h"
#include "textures/button_c_im_pp.h"
#include "textures/button_c_im_mm.h"
#include "textures/button_c_re_pp.h"
#include "textures/button_c_re_mm.h"

static SDL_Window *win = NULL;

#define BUTTON_SECTION_COLOR 177

#define BUTTON_SEC_Y 55

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
         *(px + scr->format->Rshift/8) = *(img++);
         *(px + scr->format->Gshift/8) = *(img++);
         *(px + scr->format->Bshift/8) = *(img++);
      }
   }
   int i = 0;
   if (!animation) {
      for(int y = 0; y < scr->h; ++y) {
         for(int x = scr->w - BUTTON_WIN_W; x < scr->w; ++x) {
            const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
            Uint8 *px = (Uint8*)scr->pixels + idx;
            if (y % BUTTON_SEC_Y == 0) {
               i = 0;
            }
            if (y < BUTTON_SEC_Y * (REDRAW_ANIM + 1)) {
               *(px + scr->format->Rshift / 8) = butt_anim[i++];
               *(px + scr->format->Gshift / 8) = butt_anim[i++];
               *(px + scr->format->Bshift / 8) = butt_anim[i++];
            } else if (y < BUTTON_SEC_Y * (REDRAW_C_IM_P + 1)) {
               *(px + scr->format->Rshift / 8) = butt_c_im_p[i++];
               *(px + scr->format->Gshift / 8) = butt_c_im_p[i++];
               *(px + scr->format->Bshift / 8) = butt_c_im_p[i++];
            } else if (y < BUTTON_SEC_Y * (REDRAW_C_IM_M + 1)) {
               *(px + scr->format->Rshift / 8) = butt_c_im_m[i++];
               *(px + scr->format->Gshift / 8) = butt_c_im_m[i++];
               *(px + scr->format->Bshift / 8) = butt_c_im_m[i++];
            } else if (y < BUTTON_SEC_Y * (REDRAW_C_RE_P + 1)) {
               *(px + scr->format->Rshift / 8) = butt_c_re_p[i++];
               *(px + scr->format->Gshift / 8) = butt_c_re_p[i++];
               *(px + scr->format->Bshift / 8) = butt_c_re_p[i++];
            } else if (y < BUTTON_SEC_Y * (REDRAW_C_RE_M + 1)) {
               *(px + scr->format->Rshift / 8) = butt_c_re_m[i++];
               *(px + scr->format->Gshift / 8) = butt_c_re_m[i++];
               *(px + scr->format->Bshift / 8) = butt_c_re_m[i++];
            } else {
               *(px + scr->format->Rshift / 8) = 215;
               *(px + scr->format->Gshift / 8) = 205;
               *(px + scr->format->Bshift / 8) = 205;
            }
         }
      }
   } else {
      for(int y = 0; y < scr->h; ++y) {
         for(int x = scr->w - BUTTON_WIN_W; x < scr->w; ++x) {
            const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
            Uint8 *px = (Uint8*)scr->pixels + idx;
            if (y % BUTTON_SEC_Y == 0) {
               i = 0;
            }
            if (y < BUTTON_SEC_Y * (REDRAW_ANIM + 1)) {
               if (butt_anim[i] != 215) {
                  *(px + scr->format->Rshift / 8) = butt_anim[i++];
                  *(px + scr->format->Gshift / 8) = butt_anim[i++];
                  *(px + scr->format->Bshift / 8) = butt_anim[i++];
               } else {
                  *(px + scr->format->Rshift / 8) = 161;
                  *(px + scr->format->Gshift / 8) = 151;
                  *(px + scr->format->Bshift / 8) = 151;
                  i += 3;
               }
            } else if (y < BUTTON_SEC_Y * (REDRAW_C_IM_P + 1)) {
               *(px + scr->format->Rshift / 8) = butt_c_im_p[i++];
               *(px + scr->format->Gshift / 8) = butt_c_im_p[i++];
               *(px + scr->format->Bshift / 8) = butt_c_im_p[i++];
            } else if (y < BUTTON_SEC_Y * (REDRAW_C_IM_M + 1)) {
               *(px + scr->format->Rshift / 8) = butt_c_im_m[i++];
               *(px + scr->format->Gshift / 8) = butt_c_im_m[i++];
               *(px + scr->format->Bshift / 8) = butt_c_im_m[i++];
            } else if (y < BUTTON_SEC_Y * (REDRAW_C_RE_P + 1)) {
               *(px + scr->format->Rshift / 8) = butt_c_re_p[i++];
               *(px + scr->format->Gshift / 8) = butt_c_re_p[i++];
               *(px + scr->format->Bshift / 8) = butt_c_re_p[i++];
            } else if (y < BUTTON_SEC_Y * (REDRAW_C_RE_M + 1)) {
               *(px + scr->format->Rshift / 8) = butt_c_re_m[i++];
               *(px + scr->format->Gshift / 8) = butt_c_re_m[i++];
               *(px + scr->format->Bshift / 8) = butt_c_re_m[i++];
            } else {
               *(px + scr->format->Rshift / 8) = 215;
               *(px + scr->format->Gshift / 8) = 205;
               *(px + scr->format->Bshift / 8) = 205;
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
                  if (y < BUTTON_SEC_Y * (REDRAW_ANIM + 1)) {
                     if (butt_anim[i] != 215) {
                        *(px + scr->format->Rshift / 8) = butt_anim[i++];
                        *(px + scr->format->Gshift / 8) = butt_anim[i++];
                        *(px + scr->format->Bshift / 8) = butt_anim[i++];
                     } else {
                        *(px + scr->format->Rshift / 8) = 161;
                        *(px + scr->format->Gshift / 8) = 151;
                        *(px + scr->format->Bshift / 8) = 151;
                        i += 3;
                     }
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
                  if (y < BUTTON_SEC_Y * (REDRAW_ANIM + 1)) {
                     *(px + scr->format->Rshift / 8) = butt_anim[ind++];
                     *(px + scr->format->Gshift / 8) = butt_anim[ind++];
                     *(px + scr->format->Bshift / 8) = butt_anim[ind++];
                     i += 3;
                  } else {
                     break;
                  }
               }
            }
         }
         break;
      case REDRAW_C_IM_M:
         for(int y = 0; y < scr->h; ++y) {
            for(int x = scr->w - BUTTON_WIN_W; x < scr->w; ++x) {
               const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
               Uint8 *px = (Uint8*)scr->pixels + idx;
               if (y >= BUTTON_SEC_Y * REDRAW_C_IM_M && y < BUTTON_SEC_Y * (REDRAW_C_IM_M + 1)) {
                     if (butt_c_im_m[i] != 215) {
                        *(px + scr->format->Rshift / 8) = butt_c_im_m[i++];
                        *(px + scr->format->Gshift / 8) = butt_c_im_m[i++];
                        *(px + scr->format->Bshift / 8) = butt_c_im_m[i++];
                     } else {
                        *(px + scr->format->Rshift / 8) = 161;
                        *(px + scr->format->Gshift / 8) = 151;
                        *(px + scr->format->Bshift / 8) = 151;
                        i += 3;
                     }
               } else {
                  break;
               }
            }
         }
         break;
      case REDRAW_C_IM_P:
         for(int y = 0; y < scr->h; ++y) {
            for(int x = scr->w - BUTTON_WIN_W; x < scr->w; ++x) {
               const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
               Uint8 *px = (Uint8*)scr->pixels + idx;
               if (y >= BUTTON_SEC_Y * REDRAW_C_IM_P && y < BUTTON_SEC_Y * (REDRAW_C_IM_P + 1)) {
                     if (butt_c_im_p[i] != 215) {
                        *(px + scr->format->Rshift / 8) = butt_c_im_p[i++];
                        *(px + scr->format->Gshift / 8) = butt_c_im_p[i++];
                        *(px + scr->format->Bshift / 8) = butt_c_im_p[i++];
                     } else {
                        *(px + scr->format->Rshift / 8) = 161;
                        *(px + scr->format->Gshift / 8) = 151;
                        *(px + scr->format->Bshift / 8) = 151;
                        i += 3;
                     }
               } else {
                  break;
               }
            }
         }
         break;
      case REDRAW_C_RE_M:
         for(int y = 0; y < scr->h; ++y) {
            for(int x = scr->w - BUTTON_WIN_W; x < scr->w; ++x) {
               const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
               Uint8 *px = (Uint8*)scr->pixels + idx;
               if (y >= BUTTON_SEC_Y * REDRAW_C_RE_M && y < BUTTON_SEC_Y * (REDRAW_C_RE_M + 1)) {
                     if (butt_c_im_m[i] != 215) {
                        *(px + scr->format->Rshift / 8) = butt_c_re_m[i++];
                        *(px + scr->format->Gshift / 8) = butt_c_re_m[i++];
                        *(px + scr->format->Bshift / 8) = butt_c_re_m[i++];
                     } else {
                        *(px + scr->format->Rshift / 8) = 161;
                        *(px + scr->format->Gshift / 8) = 151;
                        *(px + scr->format->Bshift / 8) = 151;
                        i += 3;
                     }
               } else {
                  break;
               }
            }
         }
         break;
      case REDRAW_C_RE_P:
         for(int y = 0; y < scr->h; ++y) {
            for(int x = scr->w - BUTTON_WIN_W; x < scr->w; ++x) {
               const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
               Uint8 *px = (Uint8*)scr->pixels + idx;
               if (y >= BUTTON_SEC_Y * REDRAW_C_RE_P && y < BUTTON_SEC_Y * (REDRAW_C_RE_P + 1)) {
                     if (butt_c_im_m[i] != 215) {
                        *(px + scr->format->Rshift / 8) = butt_c_re_p[i++];
                        *(px + scr->format->Gshift / 8) = butt_c_re_p[i++];
                        *(px + scr->format->Bshift / 8) = butt_c_re_p[i++];
                     } else {
                        *(px + scr->format->Rshift / 8) = 161;
                        *(px + scr->format->Gshift / 8) = 151;
                        *(px + scr->format->Bshift / 8) = 151;
                        i += 3;
                     }
               } else {
                  break;
               }
            }
         }
         break;
   }
   SDL_UpdateWindowSurface(win);
   if (!anim_change) {
      SDL_Delay(100);

   int i = 0;
   for(int y = 0; y < scr->h; ++y) {
      for(int x = scr->w - BUTTON_WIN_W; x < scr->w; ++x) {
         const int idx = (y * (scr->w) + x) * scr->format->BytesPerPixel;
         Uint8 *px = (Uint8*)scr->pixels + idx;
         if (y % BUTTON_SEC_Y == 0) {
            i = 0;
         }
         if (y < BUTTON_SEC_Y * (REDRAW_ANIM + 1)) {
            *(px + scr->format->Rshift / 8) = butt_anim[i++];
            *(px + scr->format->Gshift / 8) = butt_anim[i++];
            *(px + scr->format->Bshift / 8) = butt_anim[i++];
         } else if (y < BUTTON_SEC_Y * (REDRAW_C_IM_P + 1)) {
            *(px + scr->format->Rshift / 8) = butt_c_im_p[i++];
            *(px + scr->format->Gshift / 8) = butt_c_im_p[i++];
            *(px + scr->format->Bshift / 8) = butt_c_im_p[i++];
         } else if (y < BUTTON_SEC_Y * (REDRAW_C_IM_M + 1)) {
            *(px + scr->format->Rshift / 8) = butt_c_im_m[i++];
            *(px + scr->format->Gshift / 8) = butt_c_im_m[i++];
            *(px + scr->format->Bshift / 8) = butt_c_im_m[i++];
         } else if (y < BUTTON_SEC_Y * (REDRAW_C_RE_P + 1)) {
            *(px + scr->format->Rshift / 8) = butt_c_re_p[i++];
            *(px + scr->format->Gshift / 8) = butt_c_re_p[i++];
            *(px + scr->format->Bshift / 8) = butt_c_re_p[i++];
         } else if (y < BUTTON_SEC_Y * (REDRAW_C_RE_M + 1)) {
            *(px + scr->format->Rshift / 8) = butt_c_re_m[i++];
            *(px + scr->format->Gshift / 8) = butt_c_re_m[i++];
            *(px + scr->format->Bshift / 8) = butt_c_re_m[i++];
         } else {
            *(px + scr->format->Rshift / 8) = 215;
            *(px + scr->format->Gshift / 8) = 205;
            *(px + scr->format->Bshift / 8) = 205;
         }
      }
      SDL_UpdateWindowSurface(win);
   }
   }

}

/* end of xwin_sdl.c */
