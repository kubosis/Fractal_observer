/*
 * File name: computation.h
 * Description: computation functions
 * Date: 05/05/2021
 * Author: Jakub Sukdol
 */

#ifndef __COMPUTATION_H__
#define __COMPUTATION_H__

#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>

#include "../io_handle/messages.h"
#include "../utils/utils.h"

/* Macros for --set call ans --put call */
#define SET_ALL 0
#define SET_C 1
#define SET_RANGE 2

#define SET_RANGE_RE 3
#define SET_RANGE_IM 4
#define SET_N 5

enum {
    JULIA_2,
    JULIA_3,
    JULIA_4,
    JULIA_5,
    JULIA_6,
    JULIA_7,
    MANDELBROT,
    MULTIBROT_3,
    MULTIBROT_4,
    MULTIBROT_5,
    MULTIBROT_6,
    MULTIBROT_7,
    CHAOS_GAME,
    BARSNLEY_FERN,
    FRACTAL_NUM
};

void computation_init();

void computation_cleanup();

bool is_computing();

bool is_done();

void abort_comp();

bool set_compute(message *msg);

bool compute(message *msg);

void update_data(const msg_compute_data *comp_data);

void get_grid_size(int *w, int *h);

void update_image(int w, int h, uint8_t *img);

void reset_cid();

void comp_ended();

void comp_grid_resize(int w, int h);

bool cpu_comp(void);
bool comp_barnsley(void);
bool comp_chaos(void);
bool (*cpu_compute)(void);

uint16_t (*fractal)(double re, double im, int n, double c_re, double c_im);

void set_init(int set);

void control(int set);

void clear_buffer(uint8_t *img);

void comp_animation(double offset);

void zoom(bool increase, bool drawn);

void zoom_scroll(bool increase, bool drawn, int x, int y);

void change_complex(bool increase, bool real, bool drawn);

void disp_parms();

void move_up_down(bool up, bool drawn);

void move_right_left(bool right, bool drawn);

void set_params(int param, void *val1, void *val2); // without param we don't know if vals are range or c

int get_x_max();


#endif
