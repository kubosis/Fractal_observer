/*
 * File name: computation.c
 * Description: computation functions
 * Date: 05/05/2021
 * Author: Jakub Sukdol
 */

#include <math.h>
#include <omp.h>
#include <time.h>

#include "computation.h"
#include "../utils/utils.h"
#include "../gui_win/gui.h"
#include "../io_handle/iothreads.h"
#include "fractal.h"

#define ZOOM_INCREMENT 1.4
#define ITERATIONS_INCREMENT 0.85
#define COMPLEX_INCREMENT 0.01
#define MOVE_INCREMENT -0.05
#define ROUND_ERROR 0.000001

#define M_PI 3.14159265358979323846

static struct {
    double c_re;
    double c_im;
    int n;

    double range_re_min;
    double range_re_max;
    double range_im_min;
    double range_im_max;

    double tmp_re_min;
    double tmp_re_max;
    double tmp_im_min;
    double tmp_im_max;

    int grid_w;
    int grid_h;

    int cur_x;
    int cur_y;

    double d_re;
    double d_im;

    int nbr_chunks;
    int chunk_id;
    double chunk_re;
    double chunk_im;

    uint8_t chunk_n_re;
    uint8_t chunk_n_im;

    uint8_t *grid;
    bool computing;
    bool done;

} comp = {  // basic values for computation, run with -set to change them
    .c_re = -0.45,
    .c_im = 0.6,
    .n = 60,

    .range_re_min = -1.6,   // basic values
    .range_re_max = 1.6,    // run with --set
    .range_im_min = -1.6,   // to change all of them
    .range_im_max = 1.6,    // or with --help to see
                            // other options
    .grid = NULL,
    .grid_w = 640,  // medium sized window
    .grid_h = 480,  // (just my preference...)

    .chunk_n_re = 64,
    .chunk_n_im = 48,

    .computing = false,
    .done = false,

    .chunk_id = 0,

};

void computation_init()
{
    set_fractal(JULIA_2, NULL);
    presets_init();
    fr = JULIAN_AND_MANDELBROT;
    chaos_preset = TRIANGLE;
    chaos_col = COLORS;
    set_chaos_preset(chaos_preset, chaos_col);
    
    comp.grid = allocate_mem(comp.grid_w * comp.grid_h);
    barn_grid.grid = allocate_mem(comp.grid_w * comp.grid_h);

    barn_grid.w = comp.grid_w;
    barn_grid.h = comp.grid_h;
    barn_grid.done = false;

    comp.d_re = (comp.range_re_max - comp.range_re_min) / (1. * comp.grid_w);
    comp.d_im = -(comp.range_im_max - comp.range_im_min) / (1. * comp.grid_h);

    comp.nbr_chunks = (comp.grid_h * comp.grid_w) / (comp.chunk_n_re * comp.chunk_n_im);
}

void computation_cleanup()
{
    if (comp.grid) {
        free(comp.grid);
    }
    comp.grid = NULL;
}

bool is_computing()
{
    return comp.computing;
}

bool is_done()
{
    return comp.done;
}

void abort_comp()
{
    comp.computing = false;
}

bool set_compute(message *msg)
{
    bool ret = !is_computing();

    if (ret) {
        int rest_re = comp.grid_w % comp.chunk_n_re == 0? 0 : 1;
        int chunks_re = comp.grid_w / comp.chunk_n_re + rest_re;
        int rest_im = comp.grid_h % comp.chunk_n_im == 0? 0 : 1;
        int chunks_im = comp.grid_h / comp.chunk_n_im + rest_im;

        comp.nbr_chunks = chunks_re * chunks_im;

        msg->type = MSG_SET_COMPUTE;
        msg->data.set_compute.c_re = comp.c_re;
        msg->data.set_compute.c_im = comp.c_im;
        msg->data.set_compute.d_re = comp.d_re;
        msg->data.set_compute.d_im = comp.d_im;
        msg->data.set_compute.n = comp.n;
        comp.done = false;
    }
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Set new computation resolution %dx%d no. of chunks: %d\n",
        comp.grid_w, comp.grid_h, comp.nbr_chunks);
    return ret;
}

bool compute(message *msg)
{
    int tmp_n_re = comp.chunk_n_re;
    int tmp_n_im = comp.chunk_n_im;
    if (comp.chunk_id == 0) { // first chunk
        comp.computing = true;
        comp.cur_x = comp.cur_y = 0; // upper left corner
        comp.chunk_re = comp.range_re_min; // upper
        comp.chunk_im = comp.range_im_max; // left
        msg->type = MSG_COMPUTE;
    } else { // other chunks
        if (comp.chunk_id < comp.nbr_chunks) {
            comp.cur_x += comp.chunk_n_re;
            comp.chunk_re += comp.chunk_n_re * comp.d_re;
            if (comp.cur_x >= comp.grid_w) { // cursor out of image on x axes
                comp.cur_x = 0;
                comp.chunk_re = comp.range_re_min;
                comp.chunk_im += comp.chunk_n_im * comp.d_im;
                comp.cur_y += comp.chunk_n_im;
            } 
            if (comp.cur_x + comp.chunk_n_re > comp.grid_w) {   // not enough space for whole chunk
                comp.chunk_n_re = comp.grid_w - comp.cur_x;
            }
            msg->type = MSG_COMPUTE;
        }
    }

    if (comp.computing && msg->type == MSG_COMPUTE) {
        comp.chunk_id += 1;
        msg->data.compute.cid = comp.chunk_id;
        msg->data.compute.re = comp.chunk_re;
        msg->data.compute.im = comp.chunk_im;
        msg->data.compute.n_re = comp.chunk_n_re;
        msg->data.compute.n_im = comp.chunk_n_im;
        fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " New computation chunk id: %d out of %d for part %d x %d\r", 
            msg->data.compute.cid - 1, comp.nbr_chunks, comp.grid_w, comp.grid_h);
    }

    comp.chunk_n_re = tmp_n_re;
    comp.chunk_n_im = tmp_n_im;

    return is_computing();
}

void update_data(const msg_compute_data *comp_data)
{
    if (comp_data->cid == comp.chunk_id) {
        const int idx = comp.cur_x + comp_data->i_re + (comp.cur_y + comp_data->i_im) * comp.grid_w;
        if (idx >= 0 && idx < (comp.grid_h * comp.grid_w)) {
            comp.grid[idx] = comp_data->iter;
        }
        if ((comp.chunk_id + 1) > comp.nbr_chunks && (comp_data->i_re + 1) == comp.chunk_n_re && (comp_data->i_im + 1 == comp.chunk_n_im)) {
            comp.done = true;
            comp.computing = false;
            
            fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Computation of  %d out of %d chunks for part %d x %d is done\r", 
                comp.nbr_chunks, comp.nbr_chunks, comp.grid_w, comp.grid_h);
            comp.chunk_id = 0;
            fprintf(stderr, "\n");
        }

    } else {
        warning("Recieved data with unexpected chunk id");
    }
}

void get_grid_size(int *w, int *h)
{
    *w = comp.grid_w;
    *h = comp.grid_h;
}

void update_image(int w, int h, uint8_t *img)
{
    int img_idx = 0;
    int R,G,B;
    int color[][3] = {{255,0,0},  {0,255,0}, {0,0,255}, {255,255,0}, {255,0,255}, {0,255,255}};
    // all basic colors with their complement
    switch (fr) {
        case JULIAN_AND_MANDELBROT:
            for (int i = 0; i < w * h; ++i) {
                int R, G, B;
                const double t = 1. * comp.grid[i] / (comp.n);
                R = 8.5 * (1-t)*t*t*t * 255;
                G = 14 * (1-t)*(1-t)*t * t * 255;
                B = 9 * (1-t)*(1-t)*(1-t) * t * 255;
                img[img_idx++] = R <= 255 ? (R >= 0 ? R : 0) : 255;
                img[img_idx++] = G <= 255 ? (G >= 0 ? G : 0) : 255;
                img[img_idx++] = B <= 255 ? (B >= 0 ? B : 0) : 255;

            }
            break;
        case BARNSLEY:
            for (int i = 0; i < w * h; ++i) {
                img[img_idx++] = comp.grid[i] == 255? 0 : 255;
                img[img_idx++] = 255;
                img[img_idx++] = comp.grid[i] == 255? 0 : 255;
            }
            break;
        case CHAOS:
            for (int i = 0; i < w * h; ++i) {
                if (comp.grid[i] == 0) {
                    img[img_idx++] = 255;
                    img[img_idx++] = 255;
                    img[img_idx++] = 255;
                } else {
                    if (fr_data.ch_d.colors == COLORS) {
                        R = color[comp.grid[i]][0];
                        G = color[comp.grid[i]][1];
                        B = color[comp.grid[i]][2];
                    } else if (fr_data.ch_d.colors == CYAN) {
                        R = 0;
                        G = 255;
                        B = 255;
                    } else {
                        R = rand()%255;
                        G = rand()%255;
                        B = rand()%255;
                    }
                    img[img_idx++] = R;
                    img[img_idx++] = G;
                    img[img_idx++] = B;
                }
            }
            break;
        default:
            break;
    }
}

void reset_cid()
{
    comp.chunk_id -= 1;
}

void comp_ended()
{
    comp.computing = false;
    comp.chunk_id = 0;
}

void comp_grid_resize(int w, int h)
{
    barn_grid.h = comp.grid_h = h;
    barn_grid.w = comp.grid_w = w;
    free(comp.grid);
    free(barn_grid.grid);
    computation_init();
}

bool cpu_comp()
{
    bool done = false;
    double re = comp.range_re_min;
    double im = comp.range_im_max;
    for (int i = 0; i < comp.grid_h * comp.grid_w; ++i) {
        comp.grid[i] = fractal(re, im, comp.n, comp.c_re, comp.c_im);
        re += comp.d_re;
        if (re >= comp.range_re_max - ROUND_ERROR) {
            re = comp.range_re_min;
            im += comp.d_im;
        }
    }
    done = true;
    return done;
}

bool comp_barnsley()
{
    comp.grid = barn_grid.grid;
    gui_refresh();
    return true;
}

bool comp_chaos(void)
{
	time_t t;
	int side, vertices[fr_data.ch_d.verticles][3],seedX,seedY;
	int i,iter,choice;
 
	side = comp.grid_w / fr_data.ch_d.ratio;
 
	iter = 200000000;

    for (int i = 0; i < comp.grid_w * comp.grid_h; i++) {
        comp.grid[i] = 0;
    }
 
	for(i=0;i<fr_data.ch_d.verticles;i++){
		vertices[i][0] = ((int)(comp.grid_w/2 + side*cos(i*2*M_PI/fr_data.ch_d.verticles)));
		vertices[i][1] = ((int)(comp.grid_h/2 + side*sin(i*2*M_PI/fr_data.ch_d.verticles)));
	}
 
	srand((unsigned)time(&t));
 
	seedX = 0;
	seedY = 0;

    int color[] = {0,1,2,3,4,5};
    int col_shift = rand() % 6;
    int col_ind = 1;
 
	for(i=0;i<iter;i++){
		choice = rand()%fr_data.ch_d.verticles;
 
		seedX = (seedX + vertices[choice][0])/fr_data.ch_d.mod + comp.grid_w*fr_data.ch_d.shift;
		seedY = (seedY + vertices[choice][1])/fr_data.ch_d.mod + comp.grid_h*fr_data.ch_d.shift;

        col_ind = (choice) % 6;
		comp.grid[(int)(seedX +  comp.grid_w * seedY)] = color[col_ind];
        if (i%5000000 == 0) {
            col_shift += 1;
            fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Sierpenski chaos is being calculated - progress = %d%%\r", i*100/200000000);
            gui_refresh();
        }
	}
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Sierpenski chaos being calculated - progress = %d%%\r\n", 100);
    return true;
}

void set_init(int set)
{
    printf("---------------------------------------------------------------------------\n");
    help("Set the parameters:");

    if (set == SET_ALL || set == SET_C) {
        printf("Input real part of c  ('" ANSI_INFO "c_re" ANSI_RESET "'): ");
        if (scanf("%le", &comp.c_re) != 1) {
            error("Cannot scan argument");
            exit(100);
        }
        printf("Input imaginary part of c  ('" ANSI_INFO "c_im" ANSI_RESET "'): ");
        if (scanf("%le", &comp.c_im) != 1) {
            error("Cannot scan argument");
            exit(100);
        }
    }

    if (set == SET_ALL || set == SET_RANGE) {
        printf("Input maximum on real axis  ('" ANSI_INFO "range_re_max" ANSI_RESET "'): ");
        if (scanf("%le", &comp.range_re_max) != 1) {
            error("Cannot scan argument");
            exit(100);
        }
        printf("Input minimum on real axis  ('" ANSI_INFO "range_re_min" ANSI_RESET "'): ");
        if (scanf("%le", &comp.range_re_min) != 1) {
            error("Cannot scan argument");
            exit(100);
        }
        printf("Input maximum on imaginary axis  ('" ANSI_INFO "range_im_max" ANSI_RESET "'): ");
        if (scanf("%le", &comp.range_im_max) != 1) {
            error("Cannot scan argument");
            exit(100);
        }
        printf("Input minimum on imaginary axis  ('" ANSI_INFO "range_im_min" ANSI_RESET "'): ");
        if (scanf("%le", &comp.range_im_min) != 1) {
            error("Cannot scan argument");
            exit(100);
        }
        // chcek if user haven't input invalid values
        control(set);
    }

    if (set == SET_ALL) {
        printf("Input number of elements  ('" ANSI_INFO "n" ANSI_RESET "'): ");
        if (scanf("%d", &comp.n) != 1) {
            error("Cannot scan argument");
            exit(100);
        }
    }
    printf("---------------------------------------------------------------------------\n");
    info("Parametres set succesfully");
}

void control(int set)
{
    if ((comp.range_re_max <= comp.range_re_min) || (comp.range_im_max <= comp.range_im_min)) {
        warning("range max have to be bigger than range min");
        info("please input all values once again and correctly");
        set_init(set);
    }
}

void clear_buffer(uint8_t *img)
{
    comp.chunk_id = 0;
    for (int i = 0; i < comp.grid_w * comp.grid_h; ++i) {
        comp.grid[i] = 0;
    }
    for (int i = 0; i < comp.grid_w * comp.grid_h * 3; ++i) {
        img[i] = 0;
    }
}

void comp_animation(double offset)
{
    comp.c_re += offset;
    comp.c_im += offset;
    cpu_compute();
}

void zoom(bool increase, bool drawn)
{
    double increment = ZOOM_INCREMENT;

    comp.range_re_max *= increase? 1/increment : increment;
    comp.range_re_min *= increase? 1/increment : increment;
    comp.range_im_max *= increase? 1/increment : increment;
    comp.range_im_min *= increase? 1/increment : increment;
    comp.n *= increase? 1/ITERATIONS_INCREMENT : ITERATIONS_INCREMENT;
    // update densities
    comp.d_re = ((comp.range_re_max - comp.range_re_min) / (comp.grid_w));
    comp.d_im = -((comp.range_im_max - comp.range_im_min) / (comp.grid_h));

    if (drawn) {
        cpu_compute();
    }
    gui_refresh();
}
void change_complex(bool increase, bool real, bool drawn) 
{
    double increment = COMPLEX_INCREMENT;
    if (real) {
        // change real parto of c
        comp.c_re += increase? increment : -increment;
    } else {
        // change imaginary part of c
        comp.c_im += increase? increment : -increment;
    }

    if (drawn) {
        cpu_compute();
    }
    gui_refresh();
}

void disp_parms()
{
    info("Displaying actual parameters");
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " c_re = %2f\n", comp.c_re);
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " c_im = %2f\n", comp.c_im);
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " range_re_max = %2f\n", comp.range_re_max);
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " range_re_min = %2f\n",  comp.range_re_min);
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " range_im_max = %2f\n", comp.range_im_max);
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " range_im_min = %2f\n",  comp.range_im_min);
}

void move_up_down(bool up, bool drawn)
{
    comp.range_im_max += up? -MOVE_INCREMENT : MOVE_INCREMENT;
    comp.range_im_min += up? -MOVE_INCREMENT : MOVE_INCREMENT;
    if (drawn) {
        cpu_compute();
    }
    gui_refresh();
}

void move_right_left(bool right, bool drawn)
{
    comp.range_re_max += right? -MOVE_INCREMENT : MOVE_INCREMENT;
    comp.range_re_min += right? -MOVE_INCREMENT : MOVE_INCREMENT;
    if (drawn) {
        cpu_compute();
    }
    gui_refresh();
}

void set_params(int param, void *val1, void *val2)
{
    switch (param) {
        case SET_C:
            comp.c_re = *(double*)val1;
            comp.c_im = *(double*)val2;
            break;
        case SET_RANGE_RE:
            comp.range_re_min = *(double*)val1;
            comp.range_re_max = *(double*)val2;
            break;
        case SET_RANGE_IM:
            comp.range_im_min = *(double*)val1;
            comp.range_im_max = *(double*)val2;
            break;
        case SET_N:
            comp.n = *(int*)val1;
            break;
    }
    if (comp.range_im_min >= comp.range_im_max || comp.range_re_min >= comp.range_re_max) {
        error("invalid values for range");
        exit(101);
    } else {
        char *c = param == SET_C ? "C" : param == SET_RANGE_RE ? "range_re" : param == SET_RANGE_IM ? "range_im" : "number of iterations";
        fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " %s set succesfully\n", c);
    }
}

int get_x_max()
{
    return comp.grid_w;
}
