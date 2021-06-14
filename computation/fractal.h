#ifndef FRACTAL_H
#define FRACTAL_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    JULIAN_AND_MANDELBROT,
    BARNSLEY,
    CHAOS,
    FRACTAL_TYPE_NUM
} fractal_t;

typedef enum {
    CYAN,
    RANDOM,
    COLORS
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

void set_fractal(int num);
void presets_init();

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

#endif
