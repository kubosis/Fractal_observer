#ifndef __GUI_WIN_THR__
#define __GUI_WIN_THR__

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

/*
 * File name: gui_win_thread.h
 * Date:      05/10/2020
 * Author:    Jakub Sukdol
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <pthread.h>

#include <termios.h>
#include <unistd.h>

#include "../io_handle/prg_serial_nonblock.h"
#include "../io_handle/messages.h"
#include "../event_handle/event_queue.h"
#include "../io_handle/iothreads.h"

/* basically keyboard thread for gui */
void *gui_win_thread(void *v);

/* keyboard events */
bool gui_keyboard_ev(SDL_Event *sdl_ev, event *ev, bool *resize, bool *animate, data_t *data);

/* handle change menu for user inputs */
void handle_menu(char c);

#endif
