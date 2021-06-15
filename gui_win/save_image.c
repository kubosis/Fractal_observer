#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "save_image.h"

void save_image(uint8_t *image, int w, int h, char* name)
{
    char in_im[200] ;
    strcat(in_im, "/home/kubos/Documents/Fractal_observer/gui_win/");
    strcat(in_im, name);
    strcat(in_im, ".ppm");
    FILE *f_out = fopen(in_im, "wb");
    fprintf(f_out, "P6\n%d\n%d\n255\n",w,h);
    fwrite(image,w*h*3, sizeof(uint8_t),f_out);
    fclose(f_out);
    
    char cmd[100];
    strcpy( cmd, "python3 /home/kubos/Documents/Fractal_observer/gui_win/compress.py ");
    strcat(cmd,name);

    int r = system(cmd);
    if(r){

    }
}