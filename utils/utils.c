/*
 * File name: utils.c
 * Description: contains useful
 * utilities and functions
 * Date: 05/05/2021
 * Author: Jakub Sukdol
 */

#include <termios.h>
#include <unistd.h>

#include "utils.h"
#include "../io_handle/iothreads.h"

void *allocate_mem(size_t size)
{
    void *ret = (void *)malloc(size);
    if (ret == NULL) {
        // malloc failed
        fprintf(stderr, "ERROR: cannot allocate mameory\n");
        exit(100);
    }
    return ret;
}

void *realloc_mem(size_t size, void *ptr)
{
    void *ret = realloc(ptr, size);
    if (ret == NULL) {
        // realloc failed
        fprintf(stderr, "ERROR: cannot allocate mameory\n");
        exit(100);
    }
    return ret;
}

void info(char *str)
{
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " %s\n", str);
}

void error(char *str)
{
    fprintf(stderr, ANSI_ERROR "ERROR:" ANSI_RESET " %s\n", str);
}

void warning(char *str)
{
    fprintf(stderr, ANSI_WARN "WARN: " ANSI_RESET " %s\n", str);
}

void debug(char *str)
{   
    fprintf(stderr, ANSI_DEBUG "DEBUG:" ANSI_RESET " %s\n", str);
}

void help(char *str)
{
    fprintf(stdout, ANSI_HELP "HELP: " ANSI_RESET " %s\n", str);
}

void print_help()
{
    printf("---------------------------------------------------------------------------\n");
    help("standart app options:                                              -");
    help("Press '" ANSI_INFO "g" ANSI_RESET "' to get firmware version of Nucleo                        -");
    help("Press '" ANSI_INFO "s" ANSI_RESET "' to set computing parameters                              -");
    help("Press '" ANSI_INFO "1" ANSI_RESET "' to start computation                                     -");
    help("Press '" ANSI_INFO "a" ANSI_RESET "' to abort computation                                     -");
    help("Press '" ANSI_INFO "r" ANSI_RESET "' to reset chunk id                                        -");
    help("Press '" ANSI_INFO "l" ANSI_RESET "' to delete actual buffer                                  -");
    help("Press '" ANSI_INFO "p" ANSI_RESET "' to resfresh GUI                                          -");
    help("Press '" ANSI_INFO "c" ANSI_RESET "' to compute fractal on pc                                 -");
    help("Press '" ANSI_INFO "i" ANSI_RESET "' to save a screenshot                                     -");
    help("Press '" ANSI_INFO "m" ANSI_RESET "' to start an animation or stop                            -");
    help("Press '" ANSI_INFO "o" ANSI_RESET "' to enter chnge parameters menu                           -");
    help("Press '" ANSI_INFO "b" ANSI_RESET "' to change baud rate                                      -");
    help("Press '" ANSI_INFO "+" ANSI_RESET "' to increase dimensions of window                         -");
    help("Press '" ANSI_INFO "-" ANSI_RESET "' to decrease dimensions of window                         -");
    help("Press '" ANSI_INFO "q" ANSI_RESET "' to quit                                                  -");
    help("Use '" ANSI_INFO "scroll wheel" ANSI_RESET "' to zoom                                         -");
    printf("---------------------------------------------------------------------------\n");
}

void print_help_start()
{
    printf("---------------------------------------------------------------------------\n");
    help("Execution options:                                                 -");
    help("Run with '" ANSI_INFO "--help" ANSI_RESET "' to see help                                      -");
    help("Run with '" ANSI_INFO "--set" ANSI_RESET "' to set all computation parameters                 -");
    help("Run with '" ANSI_INFO "--set-c" ANSI_RESET "' to set c parameter                              -");
    help("Run with '" ANSI_INFO "--set-range" ANSI_RESET "' to set range of computation                 -");
    help("Run with '" ANSI_INFO "(path to serial)" ANSI_RESET "' to set your own serial port            -");
    help("Run with '" ANSI_INFO "--put-c 'c_re' 'c_im' -ok" ANSI_RESET "' to set parameter c values     -");
    help("Run with '" ANSI_INFO "--put-range-re 'range_re_min' 'range_re_max' -ok" ANSI_RESET "' to set -");
    fprintf(stdout, "       range of real axis                                                 -\n");
    help("Run with '" ANSI_INFO "--put-range-im 'range_im_min' 'range_im_max' -ok" ANSI_RESET "' to set -");
    fprintf(stdout, "       range of imaginary axis                                            -\n");
    help("Run with '" ANSI_INFO "--put-n 'iter_num'" ANSI_RESET "' to set number of iterations          -");
    printf("---------------------------------------------------------------------------\n");
    help("Key options in changes manu (activated with 'o')                   -");
    help("Press '" ANSI_INFO "c" ANSI_RESET "' to enter C changing options                              -");
    help("Press '" ANSI_INFO "r" ANSI_RESET "' to enter range changing options                          -");
    help("Press '" ANSI_INFO "b" ANSI_RESET "' to go back                                               -");
    printf("---------------------------------------------------------------------------\n");
    help("Key options in C changing menu and range changing menu are         -");
    fprintf(stdout, "       displayed after you enter those menus                              -\n");
}

void set_raw(int reset)
{
    static struct termios tio, tioOld;
    tcgetattr(STDIN_FILENO, &tio);
    if (reset) {
        tcsetattr(STDIN_FILENO, TCSANOW, &tioOld);
    } else {
        tioOld = tio; //backup
        cfmakeraw(&tio);
        tio.c_oflag |= OPOST;
        tcsetattr(STDIN_FILENO, TCSANOW, &tio);
    }
    // inform the user about changes
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Terminal set to %s mode\n", reset == TERMINAL_RAW? "raw" : "normal");
}

void print_change_info(bool change_r) 
{
    if (change_r) {
        help("Press '" ANSI_INFO "+" ANSI_RESET "' to zoom in");
        help("Press '" ANSI_INFO "-" ANSI_RESET "' to zoom out"); 
    } else {
        help("Press '" ANSI_INFO "+" ANSI_RESET "' to increase value of real part");
        help("Press '" ANSI_INFO "-" ANSI_RESET "' to decrease value of real part");
        help("Press '" ANSI_INFO "1" ANSI_RESET "' to increase value of imaginary part");
        help("Press '" ANSI_INFO "0" ANSI_RESET "' to decrease value of imaginary part");
    }
    help("Press '" ANSI_INFO "w" ANSI_RESET "' to move up");
    help("Press '" ANSI_INFO "a" ANSI_RESET "' to move left");
    help("Press '" ANSI_INFO "s" ANSI_RESET "' to move down");
    help("Press '" ANSI_INFO "d" ANSI_RESET "' to move right");
    help("Press '" ANSI_INFO "v" ANSI_RESET "' to display actual values");
    help("Press '" ANSI_INFO "o" ANSI_RESET "' to save changes and go back");
} 

