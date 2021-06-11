/*
 * File name: gui.c
 * Description: works with gui
 * Date: 05/05/2021
 * Author: Jakub Sukdol
 */

#include <stdbool.h>
#include <inttypes.h>

#include "xwin_sdl.h"
#include "../computation/computation.h"
#include "gui.h"
#include "../utils/utils.h"


/* definition of gui window sizes */
#define RESIZE_W    80
#define RESIZE_H    60

#ifndef SCREENRES 
#define SCREENRES 1920 * 1080 * 3
#endif

static struct {
  int w;
  int h;
  uint8_t img[SCREENRES];
} gui = { .img = {0} };

void gui_init()
{
    get_grid_size(&gui.w, &gui.h);
    xwin_init(gui.w, gui.h);
}

void gui_cleanup()
{
    xwin_close();
}

void gui_refresh()
{
    update_image(gui.w, gui.h, gui.img);
    xwin_redraw(gui.w, gui.h, gui.img);
}

void gui_resize(bool increase, bool *drawn) 
{
    if (increase == false && (gui.w - RESIZE_W <= 0 || gui.h - RESIZE_H <= 0)) {
        warning("Reached minimal size");
    } else if (increase == true && (((gui.w + RESIZE_W) * (gui.h + RESIZE_H)) >= (SCREENRES / 3))) {
        warning("Reached maximal size");
    } else {
        gui.w += increase ? RESIZE_W : -RESIZE_W;
        gui.h += increase ? RESIZE_H : -RESIZE_H;
    }
   
    xwin_poll_events();
    xwin_resize(gui.w, gui.h);
    comp_grid_resize(gui.w, gui.h);
    
    if (!*drawn) {
        clear_image();
    } else {
        cpu_compute();
    }
    gui_refresh();
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET  " Window is has been resized to %d x %d succesufully\n", gui.w, gui.h);
    
}

void clear_image()
{
    clear_buffer(gui.img);
}

void img_update_request()
{
    update_image(gui.w, gui.h, gui.img);
    gui_refresh();
    xwin_poll_events();
}

void save_blank_img()
{
    warning("Do you really want to save blank image?");
    help("Press '" ANSI_INFO "y" ANSI_RESET "' for yes");
    help("Press '" ANSI_INFO "n" ANSI_RESET "' for no");
}

void gui_resize_win(int w, int h, bool drawn)
{
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET  " Window is being resized to %d x %d\n", w, h);
    xwin_poll_events();
    gui.w = w - BUTTON_WIN_W;
    gui.h = h;
    comp_grid_resize(gui.w, gui.h);
    if (!drawn) {
        clear_image();
    } else {
        cpu_compute();
    }
    gui_refresh();
}

void change_img_size(int w, int h)
{
    gui.w = w;
    gui.h = h;
    xwin_resize(gui.w, gui.h);
    gui_refresh();
}

