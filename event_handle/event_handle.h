/*
 * File name: event_handle.h
 * Date:      05/10/2020
 * Author:    Jakub Sukdol
 */


#ifndef __EV_HANDLE__
#define __EV_HANDLE__

#include "event_queue.h"
#include "../io_handle/messages.h"
#include "../io_handle/iothreads.h"

/* handle keyboard events */
bool handle_keyboard(event *ev, message *msg, bool *drawn, data_t *data);

/* handle events from nucleo */
void handle_nucleo(event *ev, bool *drawn, data_t* data);

/* Nucleo is already computing - just tell him to compute another chunk
 * And also handles changes from change menu  (zoom and change of c) */
void handle_computing(event *ev, message *msg, data_t *data, bool drawn);

/* basically just scroll up and down */
void handle_mouse(event *ev, data_t *data, bool drawn);

/* resized window events */
void handle_window(event *ev, data_t *data, bool drawn);

#endif
