/*
 * File name: prgsem.c
 * Description: The main file of 
 * FRACTAL OBSERVER™
 * Date: 05/05/2021
 * Author: Jakub Sukdol
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <pthread.h>

#include "gui_win/xwin_sdl.h"
#include "io_handle/prg_serial_nonblock.h"
#include "io_handle/messages.h"
#include "event_handle/event_queue.h"
#include "io_handle/iothreads.h"
#include "gui_win/guithreads.h"
#include "computation/computation.h"
#include "gui_win/gui.h"
#include "event_handle/event_handle.h"
#include "gui_win/gui_win_thread.h"
#include "utils/utils.h"

/* the main function */
int main(int argc, char *argv[])
{
    // initialize shared data
    data_t data = { .quit = false, .fd = -1, .animate = false, .ser_name = NULL, .baud_change = false };
    pthread_mutex_init(&mtx, NULL);
    
    // no arguments --> open serial on predefined location
    const char *serial = "/dev/ttyACM0";

    // handle program call arguments
    if (argc > 1) {

        for (int i = 1; i < argc; ++i) {
            if (strcmp(argv[i], "--help") == 0) {
                // means help 
                print_help_start();
                print_help();
                return EXIT_SUCCESS;
            } else if (strcmp(argv[i], "--set") == 0) {
                set_init(SET_ALL);
            } else if (strcmp(argv[i], "--set-c") == 0) {
                set_init(SET_C);
            } else if (strcmp(argv[i], "--set-range") == 0) {
                set_init(SET_RANGE);
            } else if (strcmp(argv[i], "--put-c") == 0) {
                if (strcmp(argv[i+3], "-ok") != 0) {
                    error("Invalid syntax");
                    return EXIT_FAILURE;
                }
                double c_im = strtod(argv[i+2], &argv[i+3]);
                double c_re = strtod(argv[i+1], &argv[i+2]);
                
                set_params(SET_C, &c_re, &c_im);
            } else if (strcmp(argv[i], "--put-range-re") == 0) {
                if (strcmp(argv[i+3], "-ok") != 0) {
                    error("Invalid syntax");
                    return EXIT_FAILURE;
                }
                double max = strtod(argv[i+2], &argv[i+3]);
                double min = strtod(argv[i+1], &argv[i+2]);
                
                set_params(SET_RANGE_RE, &min, &max);
            } else if (strcmp(argv[i], "--put-range-im") == 0) {
                if (strcmp(argv[i+3], "-ok") != 0) {
                    error("Invalid syntax");
                    return EXIT_FAILURE;
                }
                double max = strtod(argv[i+2], &argv[i+3]);
                double min = strtod(argv[i+1], &argv[i+2]);
            
                set_params(SET_RANGE_IM, &min, &max);
            } else if (strcmp(argv[i], "--put-n") == 0) {
                int n = atoi(argv[i+1]);
                set_params(SET_N, &n, NULL);
            } else if (strcmp(&argv[i][0], "\\") == 0 || strcmp(&argv[i][0], "/") == 0) {
                // means user had to call with serial
                serial = argv[i];
            }
        }
    } 
    data.ser_name = serial;
    data.fd = serial_open(serial, RATE_NORMAL);
    if (data.fd == -1) {
        fprintf(stderr, ANSI_ERROR "ERROR:" ANSI_RESET " Cannot open serial port %s\n", serial);
        warning("Functions for nucleo won't work correctly");
    }

    // set terminal to raw mode
    set_raw(TERMINAL_RAW);

    // initialize threads
    const char *thread_names[] = {  "Serial comm thread", "Animation thread", "GUI window thread" }; // (can be added)"Input Thread"
    enum { SERIAL_COM, ANIMATION, GUI_WIN, NUM_THREADS }; // (can be added)INPUT

    void* (*thr_fuction[])(void *) = {  serial_thread, animation_thread, gui_win_thread }; // (can be added)in_thread
    pthread_mutex_init(&mtx, NULL);

    pthread_t threads[NUM_THREADS];
    // create all threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        int err = pthread_create(&threads[i], NULL, thr_fuction[i], &data);
        fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Create thread '%s' %s\n", thread_names[i], ( err == 0 ? "OK" : "FAIL") );
    }

    event ev; // actual event
    message msg; // actual message

    help("Press 'h' to see help");
    help("or run with '--help' to see all options");

    computation_init();
    info("Computation initialized");
    gui_init();
    clear_image();
    gui_refresh();  // so that it isnt redrawn by other apps
    info("GUI initialized");

    bool drawn = false;

    // main while cycle
    bool quit = false;

    while (!quit) {
        ev = queue_pop();
        switch (ev.source) {
            case EV_KEYBOARD:
                pthread_mutex_lock(&mtx);
                data.quit = quit = handle_keyboard(&ev, &msg, &drawn, &data);
                pthread_mutex_unlock(&mtx);
                if (msg.type != MSG_NBR) { 
                    // messge has been set
                    // send the message
                    if (!send_message(&data, &msg)) {
                        error("send_message() does not send all bytes of the message!");
                    }
                }
                break;
            case EV_NUCLEO:
                // recieved messages from nucleo
                handle_nucleo(&ev, &drawn, &data);
                break;
            case EV_COMPUTING:
                // nucleo is computing -> just send him message with new chunks
                handle_computing(&ev, &msg, &data, drawn);
                break;
            case EV_MOUSE:
                handle_mouse(&ev, &data, drawn);
                break;
            case EV_WINDOW:
                handle_window(&ev, &data, drawn);
                break;
            case EV_BUTTON:
                handle_button(&ev, &data, &drawn);
                break;
            default:
                // no other event types exist - just for sure
                break;
        } // end of main switch
    } // end of main while


    // close all threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Call join to the thread %s\n", thread_names[i]);
        int err = pthread_join(threads[i], NULL);
        fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Joining the thread %s has been %s\n", thread_names[i], (err == 0 ? "OK" : "FAILED"));
    }

    // reset terminal
    set_raw(TERMINAL_RESET);

    computation_cleanup();
    gui_cleanup();

    return EXIT_SUCCESS;
}
