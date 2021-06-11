/*
 * File name: iothreads.c
 * Description:
 * contains threads for communication
 * Date: 05/05/2021
 * Author: Jakub Sukdol
 */

#include "iothreads.h"
#include "../utils/utils.h"
#include "../gui_win/gui.h"
#include "../computation/computation.h"

/* -> input thread now in app
void *in_thread(void *v)
{
    return NULL;
    data_t *data = (data_t*)v;
    bool end = false;
    bool animate = false;
    bool resize = false;
    int c;
    event ev = { .source = EV_KEYBOARD };
    while ( !end && (c = getchar())) {
        ev.type = EV_TYPE_NUM;
        if (!change_menu && !change_c && !change_r) {
            switch(c) {
                case 'g': // get version
                    ev.type = EV_GET_VERSION;
                    break;
                case 'q': // quit
                    end = true;
                    break;
                case 'a': // abort
                    ev.type = EV_ABORT;
                    break;
                case 'r': // reset chunk id
                    ev.type = EV_RESET_CHUNK;
                    break;
                case '1':
                    ev.type = EV_COMPUTE;
                    break;
                case 's':
                    ev.type = EV_SET_COMPUTE;
                    break;
                case 'l':
                    ev.type = EV_CLEAR_BUFFER;
                    break;
                case 'p':
                    ev.type = EV_REFRESH;
                    break;
                case 'h':
                    ev.type = EV_HELP;
                    break;
                case 'c':
                    ev.type = EV_COMPUTE_CPU;
                    break;
                case '+':
                    ev.type = EV_INCREASE;
                    resize = true;
                    break;
                case '-':
                    ev.type = EV_DECREASE;
                    resize = true;
                    break;
                case 'i':
                    ev.type = EV_SAVE_IMAGE;
                    break;
                case 'm':
                    ev.type = EV_ANIMATE;
                    break;
                case 'o':
                    change_menu = true;
                    info("You have entered vaule change menu");
                    help("Press '" ANSI_INFO "c" ANSI_RESET "' to change c number");
                    help("Press '" ANSI_INFO "r" ANSI_RESET "' to change range");
                    help("Press '" ANSI_INFO "b" ANSI_RESET "' to go back without changes");
                    break;
                default:
                    // discard all other keys
                    break;
                }

                if (resize) {
                    // check if animating and want to resize
                    pthread_mutex_lock(&mtx);
                    animate = data->animate; // check for animation
                    pthread_mutex_unlock(&mtx);
                    if (animate) {
                        warning("Cannot resize while animating!");
                        // discrad event
                        ev.type = EV_TYPE_NUM;
                    }
                    resize = false;
                }
        } else {
            if (change_menu) {
                handle_menu(c);
                if (change_menu == false && c != 'b') {
                    // means user input was valid
                    print_change_info(change_r);
                    ev.source = EV_COMPUTING;
                }
            } else {
                // change complex c or range
                switch (c) {
                    case '+':
                        if (change_r) {
                            // zoom in
                            ev.type = EV_INCREASE;
                        } else {
                            ev.type = EV_PLUS_COMPLEX;
                            ev.data.param = CHANGE_REAL;
                        }
                        break;
                    case '-':
                        if (change_r) {
                            // zoom in
                            ev.type = EV_DECREASE;
                        } else {
                            ev.type = EV_MINUS_COMPLEX;
                            ev.data.param = CHANGE_REAL;
                        }
                        break;
                    case '1':
                        // only for complex number changes
                        if (change_c) {
                            ev.type = EV_PLUS_COMPLEX;
                            ev.data.param = CHANGE_IMAG;
                        }
                        break;
                    case '0':
                        // only for complex number changes
                        if (change_c) {
                            ev.type = EV_MINUS_COMPLEX;
                            ev.data.param = CHANGE_IMAG;
                        }
                        break;
                    case 'w':
                        ev.type = EV_MOVE_UP_DOWN;
                        ev.data.param = UP;
                        break;
                    case 'a':
                        ev.type = EV_MOVE_RIGHT_LEFT;
                        ev.data.param = LEFT;
                        break;
                    case 's':
                        ev.type = EV_MOVE_UP_DOWN;
                        ev.data.param = DOWN;
                        break;
                    case 'd':
                        ev.type = EV_MOVE_RIGHT_LEFT;
                        ev.data.param = RIGHT;
                        break;
                    case 'v':
                        // display actual values of parameters
                        disp_parms();
                        break;
                    case 'o':
                        info("Changes saved succesfully");
                        ev.source = EV_KEYBOARD;    // reset ev source
                        change_r = false;
                        change_c = false;
                        break;
                    default:
                        // discard all other input keys
                        break;
                }
            }
        }

        if (ev.type != EV_TYPE_NUM) { // new event
            queue_push(ev);
        }
        pthread_mutex_lock(&mtx);
        end = end || data->quit; // check for quit
        pthread_mutex_unlock(&mtx);
    }
    ev.type = EV_QUIT;
    queue_push(ev);
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Exit input thead %p\n", (void*)pthread_self());
    return NULL;
}*/

void *serial_thread(void *v)
{
    // read bytes from the serial and puts the parsed message to the queue
    data_t *data = (data_t*)v;
    uint8_t msg_buf[sizeof(message)] = {0}; // maximal buffer for all possible messages defined in messages.h
    event ev = { .source = EV_NUCLEO, .type = EV_SERIAL, .data.msg = NULL };
    ev.data.msg = (message *)calloc(1, sizeof(message));
    bool end = false;
    int msg_size = 0;
    //int msg_len;

    unsigned char c;

    while (serial_getc_timeout(data->fd, READ_TIMEOUT_MS, &c) > 0) {}; // discard garbage

    while (!end) {
        pthread_mutex_lock(&mtx);
        bool baud_change = data->baud_change;
        end = data->quit;
        pthread_mutex_unlock(&mtx);
        if (baud_change) {
            usleep(50);
            continue;
        }
        int r = serial_getc_timeout(data->fd, READ_TIMEOUT_MS, &c);
        if (r > 0) { // character has been read

            // get the size of new message
            get_message_size(c, &msg_size);
            msg_buf[0] = ev.data.msg->type = c;

            for (int i = 1; i < msg_size; ++i) {
                if (serial_getc_timeout(data->fd, READ_TIMEOUT_MS, &c) > 0) {
                    msg_buf[i] = c;
                } else {
                    error("Cannot comunicate with Nucleo");
                }
            }

            if (!parse_message_buf(msg_buf, msg_size, ev.data.msg)) {
                debug("Message wasn't parsed correctly");
            }
            // push new event to event queue
            queue_push(ev);
            // without this the messages sometimes get stuck
            // especially with higher bauds
            usleep(50);

        } else if (r == 0) { //read but nothing has been received

        } else {
            error("Cannot receive data from the serial port");
            end = true;
        }

    }
    ev.type = EV_QUIT;
    queue_push(ev);
    free(ev.data.msg);
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Exit serial_thread %p\n", (void*)pthread_self());
    return NULL;
}

bool send_message(data_t *data, message *msg)
{
    bool ret = true;

    int msg_size;
    get_message_size(msg->type, &msg_size);

    pthread_mutex_lock(&mtx);
    int fd = data->fd;
    pthread_mutex_unlock(&mtx);

    int msg_len;
    uint8_t *msg_buf = (uint8_t *)calloc(1, MSG_SIZE);
    if (msg_buf != NULL) {
        if (!fill_message_buf(msg, msg_buf, MSG_SIZE, &msg_len)) {
            ret = false;
        } else {
            for (int i = 0; i < msg_len; ++i) {
                if (serial_putc(fd, msg_buf[i]) == -1) {
                    error("Cannot send message");
                    ret = false;
                    break;
                }
            }
        }
        free(msg_buf);
    } else {
        error("Cannot allocate memory, program terminating");
        ret = false;
        pthread_mutex_lock(&mtx);
        data->quit = true;
        pthread_mutex_unlock(&mtx);

    }
    return ret;
}
