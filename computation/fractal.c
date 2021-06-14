#include <math.h>
#include <complex.h>
#include <time.h>

#include "fractal.h"
#include "computation.h"

/* chaos game fractal presets */
chaos_data triangle_preset;
chaos_data pentagon_preset;
chaos_data heptagon_preset;
chaos_data oktagon_preset;

void presets_init()
{
    triangle_preset.verticles = 3;
    triangle_preset.mod = 2;
    triangle_preset.ratio = 3;
    triangle_preset.shift = 0;

    pentagon_preset.verticles = 5;
    pentagon_preset.mod = 2.75;
    pentagon_preset.ratio = 1.8;
    pentagon_preset.shift = 0.12;

    heptagon_preset.verticles = 7;
    heptagon_preset.mod = 3.2;
    heptagon_preset.ratio = 1.6;
    heptagon_preset.shift = 0.19;

    oktagon_preset.verticles = 8;
    oktagon_preset.mod = 3.6;
    oktagon_preset.ratio = 1.25;
    oktagon_preset.shift = 0.23;
}
/* end of chaos presets */

void set_fractal(int num)
{
    switch (num) {
        case JULIA_2:
            fr = JULIAN_AND_MANDELBROT;
            cpu_compute = cpu_comp;
            fractal = fractal_julia_2;
            break;
        case JULIA_3:
            fractal = fractal_julia_3;
            break;
        case JULIA_4:
            fractal = fractal_julia_4;
            break;
        case JULIA_5:
            fractal = fractal_julia_5;
            break;
        case JULIA_6:
            fractal = fractal_julia_6;
            break;
        case JULIA_7:
            fractal = fractal_julia_7;
            break;
        case MANDELBROT:
            fractal = mandelbrot;
            break;
        case MULTIBROT_3:
            fractal = multibrot_3;
            break;
        case MULTIBROT_4:
            fractal = multibrot_4;
            break;
        case MULTIBROT_5:
            fractal = multibrot_5;
            break;
        case MULTIBROT_6:
            fractal = multibrot_6;
            break;
        case MULTIBROT_7:
            fractal = multibrot_7;
            break;
        case BARSNLEY_FERN:
            cpu_compute = comp_barnsley;
            fr = BARNSLEY;
            break;
        case CHAOS_GAME:
            fr_data.ch_d = oktagon_preset;
            fr_data.ch_d.colors = CYAN;
            fr = CHAOS;
            cpu_compute = comp_chaos;
            break;
        default:
            break;
    }  
}

uint16_t fractal_julia_2(double re, double im, int n, double c_re, double c_im)
{
    int iter;
    for (iter = 1; iter <= n; ++iter) {
        double re_old = re;
        re = re * re - im * im + c_re;
        im = 2*re_old*im + c_im;
        double z_abs = re*re + im * im;
        if (z_abs > 4) {
            break;
        }
    }
    return iter;
}

uint16_t fractal_julia_3(double re, double im, int n, double c_re, double c_im)
{
    int iter;
    for (iter = 1; iter <= n; ++iter) {
        double re_old = re;
        re = re*re*re - im*im*re*3 + c_re;
        im = 3*re_old*re_old*im - im*im*im + c_im;
        double z_abs = re*re + im * im;
        if (z_abs > 4) {
            break;
        }
    }
    return iter;
}

uint16_t fractal_julia_4(double re, double im, int n, double c_re, double c_im)
{
    int iter;
    for (iter = 1; iter <= n; ++iter) {
        double re_old = re;
        re = re*re*re*re - 6*re*re*im*im + im*im*im*im + c_re;
        im = 4*re_old*re_old*re_old*im - 4*re_old*im*im*im + c_im;
        double z_abs = re*re + im * im;
        if (z_abs > 4) {
            break;
        }
    }
    return iter;
}

uint16_t fractal_julia_5(double re, double im, int n, double c_re, double c_im)
{
    int iter;
    for (iter = 1; iter <= n; ++iter) {
        double re_old = re;
        re = re*re*re*re*re - 10*re*re*re*im*im + 5*re*im*im*im*im + c_re;
        im = 5*re_old*re_old*re_old*re_old*im - 10*re_old*re_old*im*im*im + im*im*im*im*im + c_im;
        double z_abs = re*re + im * im;
        if (z_abs > 4) {
            break;
        }
    }
    return iter;
}

uint16_t fractal_julia_6(double re, double im, int n, double c_re, double c_im)
{
    int iter;
    for (iter = 1; iter <= n; ++iter) {
        double re_old = re;
        re = re*re*re*re*re*re - 15*re*re*re*re*im*im + 15*re*re*im*im*im*im - im*im*im*im*im*im + c_re;
        im = 6*re_old*re_old*re_old*re_old*re_old*im - 20*re_old*re_old*re_old*im*im*im + 6*re_old*im*im*im*im*im + c_im;
        double z_abs = re*re + im * im;
        if (z_abs > 4) {
            break;
        }
    }
    return iter;
}

uint16_t fractal_julia_7(double re, double im, int n, double c_re, double c_im)
{
    int iter;
    for (iter = 1; iter <= n; ++iter) {
        double re_old = re;
        re = re*re*re*re*re*re*re - 21*re*re*re*re*re*im*im + 35*re*re*re*im*im*im*im - 7*re*im*im*im*im*im*im + c_re;
        im = 7*re_old*re_old*re_old*re_old*re_old*re_old*im - 35*re_old*re_old*re_old*re_old*im*im*im + 21*re_old*re_old*im*im*im*im*im - im*im*im*im*im*im*im + c_im;
        double z_abs = re*re + im * im;
        if (z_abs > 4) {
            break;
        }
    }
    return iter;
}

uint16_t mandelbrot(double re, double im, int n, double c_re, double c_im)
{
    int iter;
    double start_re = re;
    double strat_im = im;
    for (iter = 1; iter <= n; ++iter) {
        double re_old = re;
        re = re*re - im*im + start_re;
        im = 2*re_old*im + strat_im;
        double z_abs = re*re + im * im;
        if (z_abs > 4) {
            break;
        }
    }
    return iter;
}

uint16_t multibrot_3(double re, double im, int n, double c_re, double c_im)
{
    int iter;
    double start_re = re;
    double strat_im = im;
    for (iter = 1; iter <= n; ++iter) {
        double re_old = re;
        re = re*re*re - im*im*re*3 + start_re;
        im = 3*re_old*re_old*im - im*im*im + strat_im;
        double z_abs = re*re + im * im;
        if (z_abs > 4) {
            break;
        }
    }
    return iter;
}

uint16_t multibrot_4(double re, double im, int n, double c_re, double c_im)
{
    int iter;
    double start_re = re;
    double strat_im = im;
    for (iter = 1; iter <= n; ++iter) {
        double re_old = re;
        re = re*re*re*re - 6*re*re*im*im + im*im*im*im + start_re;
        im = 4*re_old*re_old*re_old*im - 4*re_old*im*im*im + strat_im;
        double z_abs = re*re + im * im;
        if (z_abs > 4) {
            break;
        }
    }
    return iter;
}

uint16_t multibrot_5(double re, double im, int n, double c_re, double c_im)
{
    int iter;
    double start_re = re;
    double strat_im = im;
    for (iter = 1; iter <= n; ++iter) {
        double re_old = re;
        re = re*re*re*re*re - 10*re*re*re*im*im + 5*re*im*im*im*im + start_re;
        im = 5*re_old*re_old*re_old*re_old*im - 10*re_old*re_old*im*im*im + im*im*im*im*im + strat_im;
        double z_abs = re*re + im * im;
        if (z_abs > 4) {
            break;
        }
    }
    return iter;
}

uint16_t multibrot_6(double re, double im, int n, double c_re, double c_im)
{
    int iter;
    double start_re = re;
    double strat_im = im;
    for (iter = 1; iter <= n; ++iter) {
        double re_old = re;
        re = re*re*re*re*re*re - 15*re*re*re*re*im*im + 15*re*re*im*im*im*im - im*im*im*im*im*im + start_re;
        im = 6*re_old*re_old*re_old*re_old*re_old*im - 20*re_old*re_old*re_old*im*im*im + 6*re_old*im*im*im*im*im + strat_im;
        double z_abs = re*re + im * im;
        if (z_abs > 4) {
            break;
        }
    }
    return iter;
}

uint16_t multibrot_7(double re, double im, int n, double c_re, double c_im)
{
    int iter;
    double start_re = re;
    double strat_im = im;
    for (iter = 1; iter <= n; ++iter) {
        double re_old = re;
        re = re*re*re*re*re*re*re - 21*re*re*re*re*re*im*im + 35*re*re*re*im*im*im*im - 7*re*im*im*im*im*im*im + start_re;
        im = 7*re_old*re_old*re_old*re_old*re_old*re_old*im - 35*re_old*re_old*re_old*re_old*im*im*im + 21*re_old*re_old*im*im*im*im*im - im*im*im*im*im*im*im + strat_im;
        double z_abs = re*re + im * im;
        if (z_abs > 4) {
            break;
        }
    }
    return iter;
}