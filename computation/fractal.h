#ifndef FRACTAL_H
#define FRACTAL_H

#include <stdint.h>
#include <stdbool.h>

#include "../io_handle/iothreads.h"

typedef enum {
    TRIANGLE,
    PENTAGON,
    HEPTAGON,
    OKTAGON,
    CHAOS_TYPE_NUM
} chaos_presets;

typedef enum {
    JULIA,
    MANDELBROT_FR,
    CHAOS,
    BARNSLEY,
    FRACTAL_TYPE_NUM
} fractal_t;

typedef enum {
    CYAN,
    RANDOM,
    COLORS,
    COLOR_NUM
} coloring_t;

typedef struct {
    int verticles;
    float mod;
    float shift;
    float ratio;
    coloring_t colors;
} chaos_data;

typedef struct {
    fractal_t frc;
    union {
        int param;
        chaos_data ch_d;
    };
} fractal_data_t;

void set_fractal(int num, data_t *data);
void presets_init();
void set_chaos_preset(chaos_presets preset, coloring_t color);

uint16_t fractal_julia_2(double re, double im, int n, double c_re, double c_im);
uint16_t fractal_julia_3(double re, double im, int n, double c_re, double c_im);
uint16_t fractal_julia_4(double re, double im, int n, double c_re, double c_im);
uint16_t fractal_julia_5(double re, double im, int n, double c_re, double c_im);
uint16_t fractal_julia_6(double re, double im, int n, double c_re, double c_im);
uint16_t fractal_julia_7(double re, double im, int n, double c_re, double c_im);
uint16_t mandelbrot(double re, double im, int n, double c_re, double c_im);
uint16_t multibrot_3(double re, double im, int n, double c_re, double c_im);
uint16_t multibrot_4(double re, double im, int n, double c_re, double c_im);
uint16_t multibrot_5(double re, double im, int n, double c_re, double c_im);
uint16_t multibrot_6(double re, double im, int n, double c_re, double c_im);
uint16_t multibrot_7(double re, double im, int n, double c_re, double c_im);

fractal_t fr;

fractal_data_t fr_data;
chaos_presets chaos_preset;
coloring_t chaos_col;

#endif
