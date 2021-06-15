/*
 * File name: gui.h
 * Description: headerfile for gui.c
 * Date: 05/05/2021
 * Author: Jakub Sukdol
 */

#ifndef __GUI_H__
#define __GUI_H__

/* window sizes */
#define LARGE_W 704
#define LARGE_H 576
#define MEDIUM_W 512
#define MEDIUM_H 384
#define SMALL_W 320
#define SMALL_H 240

/* initialize GUI */
void gui_init();

/* free all allocated memory */
void gui_cleanup();

/* refresh gui from image buffer */
void gui_refresh();

/* resize GUI */
void gui_resize(bool increase, bool *drawn);
bool increase_size();
bool decrease_size();

/* clears img and grid buffers */
void clear_image();

/* image update request for animation
 * this fction is there because guithreads.c
 * doesn't see gui struct
 */
void img_update_request();

/* ask if user want to save blank image */
void save_blank_img();

/* change values of computing parameters */
bool change_vals_menu();

/* resize with use of window */
void gui_resize_win(int w, int h, bool drawn);

/* change sizes of image for computation */
void change_img_size(int w, int h);

void req_save_image(char *name);

#endif
