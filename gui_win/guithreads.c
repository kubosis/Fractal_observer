/*
 * File name: guithreads.c
 * Description:
 * contains threads for animation
 * Date: 05/05/2021
 * Author: Jakub Sukdol
 */

#include <stdbool.h>
#include <stdlib.h>

#include "guithreads.h"
#include "../io_handle/iothreads.h"
#include "../utils/utils.h"
#include "xwin_sdl.h"
#include "../computation/computation.h"
#include "gui.h"

#define POLL_EVENT_CALL 1000

#define ANIMATIONS_LIMIT 70
#define ANIMATIONS_OFFSET 0.0125

void *animation_thread(void *v)
{
    data_t *data = (data_t*)v;
    bool end = false;
    bool animate = false;

    double offset = ANIMATIONS_OFFSET;
    int checker = 0;
    int check_increment = +1;

    while (!end) {
        pthread_mutex_lock(&mtx);
        // turn on / off animation
        animate = data->animate;
        end = data->quit;
        pthread_mutex_unlock(&mtx);

        if (animate && !end) {
            comp_animation(offset);
            img_update_request();
            checker += check_increment;
            if (checker == ANIMATIONS_LIMIT) {
                // animate back
                offset = -offset;
                check_increment = -1;
            } else if (checker == -ANIMATIONS_LIMIT) {
                offset = -offset;
                check_increment = +1;
            }
            usleep(1);
        } else {
            usleep(500);
        }
    }

    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Exit serial_thread %p\n", (void*)pthread_self());
    return NULL;
}
