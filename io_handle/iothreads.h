/*
 * File name: threads.h
 * Description: headerfile for iothreads.c,
 * contains threads
 * Date: 05/05/2021
 * Author: Jakub Sukdol
 */

#ifndef __THREADS_H__
#define __THREADS_H__

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <pthread.h>

#include <termios.h>
#include <unistd.h>

#include "prg_serial_nonblock.h"
#include "messages.h"
#include "../event_handle/event_queue.h"

typedef struct {
    bool quit;
    int fd;
    bool animate;
    const char *ser_name;
    bool baud_change;
    bool barn;
} data_t;

pthread_mutex_t mtx;

#define READ_TIMEOUT_MS 500

#define MSG_SIZE sizeof(message)

/* takes care of keyboard user input
 * not in use now, (development purpose only)
 * for user input see gui_win_thread.c
 */
void *in_thread(void *v);

/* recieves messages from nucleo */
void *serial_thread(void *v);

/* sends messages to nucleo */
bool send_message(data_t *data, message *msg);

#endif
