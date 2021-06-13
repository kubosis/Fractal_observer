/*
 * File name: event_handle.c
 * Date:      05/10/2020
 * Author:    Jakub Sukdol
 */


#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include <sys/wait.h>

#include "event_handle.h"
#include "../utils/utils.h"
#include "../gui_win/gui.h"
#include "../computation/computation.h"
#include "../gui_win/xwin_sdl.h"
#include "../gui_win/guithreads.h"
#include "../io_handle/prg_serial_nonblock.h"
#include "../io_handle/iothreads.h"

#define INCREASE true
#define DECREASE false

int baud_level = RATE_NORMAL;
bool animate;
bool abort_req = false; // abort requested
bool comp_set = false;

bool handle_keyboard(event *ev, message *msg, bool *drawn, data_t *data)
{
    bool quit = false;
    
    bool save_blank = false;
    time_t current_time;
    msg->type = MSG_NBR;
    // handle keyboard events
    switch (ev->type) {
        case EV_GET_VERSION:
            // prepare packet for get version
            msg->type = MSG_GET_VERSION;
            info("Get version requested");
            break;
        case EV_QUIT:
            quit = true;
            if (is_computing()) {
                // abort computing if needed
                msg->type = MSG_ABORT;
                // messge has been set
                // send the message
                if (!send_message(data, msg)) {
                    error("send_message() does not send all bytes of the message!");
                }
            }
            msg->type = MSG_BAUD_RESET;
            break;
        case EV_COMPUTE:
            if (drawn) {
                *drawn = false;
                //clear_image();
            }
            if (!is_computing() && comp_set) {
                compute(msg);
            } else if (is_computing()) {
                // Nucleo was already computing
                warning("Nucleo is already computing");
            } else {
                warning("Set comp parameters first");
            }
            break;
        case EV_RESET_CHUNK:
            if (is_computing()) {
                warning("Chunk reset discarded due to ongoing computation");
            } else {
                // nucleo is not computing
                reset_cid();
                info("Chunk reset done");
            }
            break;
        case EV_CHANGE_BAUD:
            if (is_computing()) {
                warning("Cannot change baud while computing");
            } else {
                msg->type = MSG_BAUD_CHANGE;
                info("Baud change request sent");
            }
            break;
        case EV_ABORT:
            if (is_computing()) {
                info("Abort requested");
                abort_req = true;
                msg->type = MSG_ABORT;
                abort_comp();
            } else {
                // nucleo is not computing
                warning("Abort requested, but Nucleo is not computing");
            }
            break;
        case EV_SET_COMPUTE:
            if (set_compute(msg)) {
                // computation set successfully -> nothing to do here
                comp_set = true;
            } else {
                warning("New computation parameters requested but it is discarded due to on ongoing computation");
            }
            break;
        case EV_REFRESH: 
            gui_refresh();
            info("GUI refreshed");
            break;
        case EV_HELP:
            print_help();
            break;
        case EV_COMPUTE_CPU:
            // computes whole fractal on pc and displays it right away
            if (!is_computing()) {
                debug("Fractal is now being computed on PC");
                if (cpu_compute()) {
                    gui_refresh();
                    *drawn = true;
                }
            } else {
                // nucleo ios computing rn
                warning("Request abort first before cpu can compute");
            }
            break;
        case EV_INCREASE:
            if (!is_computing()) {
                gui_resize(true, drawn);
            } else {
                warning("Cannot resize the window while computing");
            }
            break;
        case EV_DECREASE:
            if (!is_computing()) {
                gui_resize(false, drawn);
            } else {
                warning("Cannot resize the window while computing");
            }
            break;
        case EV_SAVE_IMAGE:
            // generate unique name for each screen shot (based on actual time)
            // so that new screenshot does not delete previous on
            if (!*drawn && !is_computing()) {
                //  means that image is completly blank
                save_blank_img();
                SDL_Event event;
                SDL_memset(&event, 0, sizeof(event)); /* or SDL_zero(event) */
                event.type = EV_SAVE_BLANK;
                SDL_PushEvent(&event);
            } 
            
            if (*drawn || is_computing() || save_blank) {
                current_time = time(&current_time);
                struct tm * time_info = localtime(&current_time);
                char time_string[9];
                strftime(time_string, sizeof(time_string), "%H:%M:%S", time_info);
                char scr_name[] = {'S', 'C', 'R', 'E', 'E', 'N', 'S', 'H', 'O', 'T',
                    '_', time_string[0], time_string[1], '_', time_string[3], time_string[4],
                    '_', time_string[6], time_string[7], '.', 'P', 'N', 'G', '\0'};
                save_image(scr_name);
                fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " %s was generated\n", scr_name);
                save_blank = false;
            }
            break;
        case EV_SAVE_BLANK:
            save_blank = true;
            event ev_new = { .source = EV_KEYBOARD, .type = EV_SAVE_IMAGE };
            queue_push(ev_new);
            break;
        case EV_CLEAR_BUFFER:
            clear_image();
            gui_refresh();
            *drawn = false;
            info("Buffer cleared succesfully");
            break;
        case EV_ANIMATE:
            // turn on / off animation
            animate = data->animate = !data->animate;
            *drawn = true;
            if (animate) {
                info("Animation has started");
            } else {
                info("Animation has ended");
            }
            break;
        default:
            // nothing to do with other events
            debug("Unknown event from keyboard");
            break;
    }
    return quit;
}

void handle_nucleo(event *ev, bool *drawn, data_t* data)
{

    switch (ev->data.msg->type) {
        case MSG_OK:
            if (!is_computing()) {
                info("Recieved OK from Nucleo");
                abort_req = false;
            }
            break;
        case MSG_STARTUP:
            fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Nucleo restarted - '%s'\n", ev->data.msg->data.startup.message);
            break;
        case MSG_ABORT:
            if (!is_computing()) {
                // abort requested but nucleo isn't computing
                warning("Abort requested by Nucleo, but Nucleo isn't computing");
            } else {
                // computing aborted
                info("Abort from Nucleo");
                abort_comp();
            }
            break;
        case MSG_VERSION:
            fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Nucleo firmware ver. %d.%d-p%d\n",
                ev->data.msg->data.version.major, ev->data.msg->data.version.minor, ev->data.msg->data.version.patch);
            break;
        case MSG_DONE:
            // inform the user, that computation is done
            // and increment the chunk ID
            gui_refresh();
            if (is_done()) {
                info("Nucleo reports the computation is done");
                comp_ended();
                *drawn = true;
            } else {
                event ev_new = { .source = EV_COMPUTING, .type = EV_COMPUTE };
                queue_push(ev_new);
            }
            break;
        case MSG_COMPUTE_DATA:
            if (is_computing() || abort_req) {
                // recieved some computed data from Nucleo
                update_data(&ev->data.msg->data.compute_data);
            } else {
                warning("WARN: Nucleo sends new data without computing");
            }
            break;
        case MSG_ERROR:
            warning("Recieved error from Nucleo");
            break;
        case MSG_BAUD_CHANGE:
            info("Baud change accepted");
            event new_ev = { .source = EV_COMPUTING,  .type = EV_CHANGE_BAUD };
            queue_push(new_ev);

            data->baud_change = true;

            break;        
        default:
            // nothing to do with other events
            debug("Unknown event from Nucleo");
            break;
    }
}

void handle_computing(event *ev, message *msg, data_t *data, bool drawn)
{
    switch (ev->type) {
        case EV_COMPUTE:
            compute(msg);
            if (!send_message(data, msg)) {
                error("send_message() does not send all bytes of the message!");
            }
            break;
        case EV_INCREASE:
            zoom(INCREASE, drawn);    // zoom in
            break;
        case EV_DECREASE:
            zoom(DECREASE, drawn);   // zoom out
            break;
        case EV_PLUS_COMPLEX:
            change_complex(INCREASE, ev->data.param, drawn);
            break;
        case EV_MINUS_COMPLEX:
            change_complex(DECREASE, ev->data.param, drawn);
            break;
        case EV_MOVE_UP_DOWN:
            move_up_down(ev->data.param, drawn);
            break;
        case EV_MOVE_RIGHT_LEFT:
            move_right_left(ev->data.param, drawn);
            break;
        case EV_CHANGE_BAUD:
            serial_close(data->fd);
            baud_level += 1; // increase baud level
            baud_level %= BAUD_NUM;
            serial_open(data->ser_name, baud_level);
            sleep(2);
            unsigned char c;
            while (serial_getc_timeout(data->fd, 80, &c)) {} // get rid of junk
            char *baud = baud_level == RATE_NORMAL ? "B115200" : baud_level == RATE_FAST1 ? "B230400" :
                baud_level == RATE_FAST2 ? "B460800" : "B576000";
            fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Serial synchronized at baud level '%s'\n", baud);
            data->baud_change = false;
            break;
        default:
            // no other computing events
            break;
    }
}

void handle_mouse(event *ev, data_t *data, bool drawn)
{
    switch (ev->type) {
        case EV_INCREASE:
            zoom(INCREASE, drawn);    // zoom in
            break;
        case EV_DECREASE:
            zoom(DECREASE, drawn);   // zoom out
            break;
        default:
            // no other computing events
            break;
    }
}

void handle_window(event *ev, data_t *data, bool drawn)
{
    gui_resize_win(ev->data.rsz.size_w, ev->data.rsz.size_h, drawn);
}

void handle_button(event *ev, data_t *data, bool *drawn)
{
    switch (ev->type) {
        case EV_ANIMATE:
            // turn on / off animation
            animate = data->animate = !data->animate;
            xwin_redraw_button(REDRAW_ANIM, animate, true);
            *drawn = true;
            if (animate) {
                info("Animation has started");
            } else {
                info("Animation has ended");
            }
            break;
        case EV_PLUS_COMPLEX:
            xwin_redraw_button(ev->data.cb.button, animate, false);
            change_complex(INCREASE, ev->data.cb.real, drawn);
            break;
        case EV_MINUS_COMPLEX:
            xwin_redraw_button(ev->data.cb.button, animate, false);
            change_complex(DECREASE, ev->data.cb.real, drawn);
        default:
            // no other button events to handle
            break;
    }
}
