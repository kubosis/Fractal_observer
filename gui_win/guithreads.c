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

void *barnsley_fern_thread(void* v)
{
    data_t *data = (data_t*)v;
    bool end = false;

    time_t t;
    srand((unsigned)time(&t));

    double x0=0,y0=0,x1,y1;
    int diceThrow;
    bool barn = false;

    barn_grid.w = 640;
    barn_grid.h = 480;
    barn_grid.grid = allocate_mem(barn_grid.w*barn_grid.h);
    bool cleaned = false;

    int index = 0;

    while (!end) {
        pthread_mutex_lock(&mtx);
        end = data->quit;
        barn = data->barn;
        pthread_mutex_unlock(&mtx);

        if (!barn) {
            cleaned = false;
        }

        if (barn && !end && !cleaned) {
            index = 0;
            for (int i = 0; i < barn_grid.w * barn_grid.h; i++) {
                barn_grid.grid[i] = 0;
            }
            cleaned = true;
        }

        if (!end && barn) {
            diceThrow = rand()%100;

            if(diceThrow==0) {
                x1 = 0;
                y1 = 0.16*y0;
            }

            else if(diceThrow>=1 && diceThrow<=7) {
                x1 = 0.85*x0 +0.04*y0;
                y1 = -0.04*x0 + 0.85 *y0 + 1.6;
            }

            else if(diceThrow>=8 && diceThrow<=15) {
                x1 = 0.2*x0 - 0.26*y0;
                y1 = 0.23*x0 + 0.22*y0 + 1.6;
            }

            else {
                x1 = -0.15*x0 + 0.28*y0;
                y1 = 0.26*x0 + 0.24*y0+0.44;
            }


            int grid_x = (int)(x1 / 4 * barn_grid.w + barn_grid.w / 2);
            int grid_y = (int)(barn_grid.h - y1 / 10 * barn_grid.h) * barn_grid.w;
            barn_grid.grid[grid_x + grid_y] = 255;

            x0 = x1;
            y0 = y1;
            index += 1;

            if (index % 10000 == 0) {
                cpu_compute();
            }
        }
    }
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Exit serial_thread %p\n", (void*)pthread_self());
    return NULL;
}
