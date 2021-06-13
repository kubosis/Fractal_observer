/*
 * File name: event_queue.h
 * Date:      2017/04/15 12:41
 * Author:    Jan Faigl && Jakub Sukdol
 */

#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__

#include "../io_handle/messages.h"


typedef enum {
   EV_NUCLEO,
   EV_KEYBOARD,
   EV_COMPUTING,
   EV_MOUSE,
   EV_WINDOW,
   EV_BUTTON,
   EV_NUM
} event_source;

typedef enum {
   EV_COMPUTE, // request compute on nucleo with particular 
   EV_RESET_CHUNK, //reset the chunk id
   EV_ABORT,
   EV_GET_VERSION,
   EV_THREAD_EXIT,
   EV_QUIT,
   EV_SERIAL,
   EV_SET_COMPUTE,
   EV_COMPUTE_CPU,
   EV_CLEAR_BUFFER,
   EV_REFRESH,
   EV_HELP,
   EV_INCREASE,   // resize when part of EV_KEYBOARD
   EV_DECREASE,   // zoom whe part of EV_COMPUTING
   EV_SAVE_IMAGE,
   EV_ANIMATE,
   EV_PLUS_COMPLEX,
   EV_MINUS_COMPLEX,
   EV_MOVE_UP_DOWN,
   EV_MOVE_RIGHT_LEFT,
   EV_RESIZE,
   EV_SAVE_BLANK,
   EV_CHANGE_BAUD,
   EV_TYPE_NUM,
} event_type;

typedef struct {
   int param;
} event_keyboard;

typedef struct {
   int button;
   bool real;
} complex_butt;

typedef struct {
   message *msg;
} event_serial;

typedef struct {
   int size_w;
   int size_h;
} resize;

typedef struct {
   event_source source;
   event_type type;
   union {
      int param;
      message *msg;
      resize rsz;
      complex_butt cb;
   } data;
} event;

void queue_init(void);
void queue_cleanup(void);

event queue_pop(void);

void queue_push(event ev);

#endif

/* end of event_queue.h */
