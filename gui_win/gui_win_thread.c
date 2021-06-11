/*
 * File name: gui_win_thread.c
 * Date:      05/10/2020
 * Author:    Jakub Sukdol
 */

#include "gui_win_thread.h"
#include "../utils/utils.h"
#include "gui.h"
#include "../computation/computation.h"

#define SDL_DELAY_MS 10

#define CHANGE_REAL true
#define CHANGE_IMAG false

#define UP true
#define DOWN false
#define RIGHT true
#define LEFT false

bool change_menu = false;   // handle events while user changes values
bool change_c = false;
bool change_r = false;
bool blank_im = false;

void *gui_win_thread(void *v)
{
    data_t *data = v;
    bool end = false;
    event ev;
    bool resize = false;
    bool animate = false;
    SDL_Event sdl_ev;
    int x,y;

    while (!end) {
        ev.source = EV_KEYBOARD;
        pthread_mutex_lock(&mtx);
        end = end || data->quit; // check for quit
        pthread_mutex_unlock(&mtx);
        ev.type = EV_TYPE_NUM;
        if (SDL_PollEvent(&sdl_ev)) {
            switch (sdl_ev.type) {
                case SDL_QUIT:
                    end = true;
                    break;
                case SDL_KEYDOWN:
                    end = gui_keyboard_ev(&sdl_ev, &ev, &resize, &animate, data);
                    break;
                case SDL_MOUSEWHEEL:
                    ev.source = EV_MOUSE;
                    if (sdl_ev.wheel.y > 0) { // scroll up
                        ev.type = EV_INCREASE;
                    } 
                    if (sdl_ev.wheel.y < 0) { // scroll down
                        ev.type = EV_DECREASE;
                    } 
                    break;
                case SDL_WINDOWEVENT:
                    if (sdl_ev.window.event == SDL_WINDOWEVENT_RESIZED) {  // window resized
                        ev.source = EV_WINDOW;
                        ev.type = EV_RESIZE;
                        ev.data.rsz.size_w = sdl_ev.window.data1;
                        ev.data.rsz.size_h = sdl_ev.window.data2;
                        usleep(1000);
                    } 
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    SDL_GetMouseState(&x, &y);
                    printf("%d %d %d\n",x,y, get_x_max());
                    int x_max = get_x_max();
                    if (x >= x_max + 15 && x <= x_max + 150) {
                        ev.source = EV_BUTTON;
                        if (y >= 15 && y <= 75) {
                            ev.type = EV_ANIMATE;
                        }
                    }
                    break;
                case EV_SAVE_BLANK:
                    blank_im = true;
                    break;
                default:
                    break;
            }
        }

        if (ev.type != EV_TYPE_NUM) {
            queue_push(ev);
        }

        SDL_Delay(SDL_DELAY_MS);
    }

    ev.type = EV_QUIT;
    queue_push(ev);
    fprintf(stderr, ANSI_INFO "INFO: " ANSI_RESET " Exit input thead %p\n", (void*)pthread_self());
    return NULL;
}

bool gui_keyboard_ev(SDL_Event *sdl_ev, event *ev, bool *resize, bool *animate, data_t *data)
{
    bool end = false;
    if (!change_menu && !change_c && !change_r && !blank_im) {
        switch (sdl_ev->key.keysym.sym) {
            case SDLK_g: // get version
                ev->type = EV_GET_VERSION;
                break;
            case SDLK_q: // quit
                end = true;
                break;
            case SDLK_a: // abort
                ev->type = EV_ABORT;
                break;
            case SDLK_r: // reset chunk id
                ev->type = EV_RESET_CHUNK;
                break;
            case SDLK_KP_1:
                ev->type = EV_COMPUTE;
                break;
            case SDLK_s:
                ev->type = EV_SET_COMPUTE;
                break;
            case SDLK_l:
                ev->type = EV_CLEAR_BUFFER;
                break;
            case SDLK_p:
                ev->type = EV_REFRESH;
                break;
            case SDLK_h:
                ev->type = EV_HELP;
                break;
            case SDLK_c:
                ev->type = EV_COMPUTE_CPU;
                break;
            case SDLK_KP_PLUS:
                ev->type = EV_INCREASE;
                *resize = true;
                break;
            case SDLK_KP_MINUS:
                ev->type = EV_DECREASE;
                *resize = true;
                break;
            case SDLK_i:
                ev->type = EV_SAVE_IMAGE;
                break;
            case SDLK_m:
                ev->type = EV_ANIMATE;
                break;
            case SDLK_o:
                change_menu = true;
                info("You have entered vaule change menu");
                help("Press '" ANSI_INFO "c" ANSI_RESET "' to change c number");
                help("Press '" ANSI_INFO "r" ANSI_RESET "' to change range");
                help("Press '" ANSI_INFO "b" ANSI_RESET "' to go back without changes");
                break;
            case SDLK_b:
                ev->type = EV_CHANGE_BAUD;
                break;
            default:
                // discard all other keys
                break;
            }

        if (*resize) {
            // check if animating and want to resize
            pthread_mutex_lock(&mtx);
            animate = &data->animate; // check for animation
            pthread_mutex_unlock(&mtx);
            if (*animate) {
                warning("Cannot resize while animating!");
                // discrad event
                ev->type = EV_TYPE_NUM;
            }
            *resize = false;
        }
    } else {
        ev->source = EV_COMPUTING;
        if (change_menu) {
            handle_menu(sdl_ev->key.keysym.sym);
            if (change_menu == false && sdl_ev->key.keysym.sym != 'b') {
                // means user input was valid
                print_change_info(change_r);
                ev->source = EV_COMPUTING;
            }
        } else if (change_r || change_c) {
            // change complex c or range
            switch (sdl_ev->key.keysym.sym) {
                case SDLK_KP_PLUS:
                    if (change_r) {
                        // zoom in
                        ev->type = EV_INCREASE;
                    } else {
                        ev->type = EV_PLUS_COMPLEX;
                        ev->data.param = CHANGE_REAL;
                    }
                    break;
                case SDLK_KP_MINUS:
                    if (change_r) {
                        // zoom in
                        ev->type = EV_DECREASE;
                    } else {
                        ev->type = EV_MINUS_COMPLEX;
                        ev->data.param = CHANGE_REAL;
                    }
                    break;
                case SDLK_KP_1:
                    // only for complex number changes
                    if (change_c) {
                        ev->type = EV_PLUS_COMPLEX;
                        ev->data.param = CHANGE_IMAG;
                    }
                    break;
                case SDLK_KP_0:
                    // only for complex number changes
                    if (change_c) {
                        ev->type = EV_MINUS_COMPLEX;
                        ev->data.param = CHANGE_IMAG;
                    }
                    break;
                case SDLK_w:
                    ev->type = EV_MOVE_UP_DOWN;
                    ev->data.param = UP;
                    break;
                case SDLK_a:
                    ev->type = EV_MOVE_RIGHT_LEFT;
                    ev->data.param = LEFT;
                    break;
                case SDLK_s:
                    ev->type = EV_MOVE_UP_DOWN;
                    ev->data.param = DOWN;
                    break;
                case SDLK_d:
                    ev->type = EV_MOVE_RIGHT_LEFT;
                    ev->data.param = RIGHT;
                    break;
                case SDLK_v:
                    // display actual values of parameters
                    disp_parms();
                    break;
                case SDLK_o:
                    info("Changes saved succesfully");
                    ev->source = EV_KEYBOARD;    // reset ev source
                    change_r = false;
                    change_c = false;
                    break;
                default:
                    // discard all other input keys
                    break;
            }
        } else {
            switch (sdl_ev->key.keysym.sym) {
                case SDLK_y:
                    ev->type = EV_SAVE_BLANK;
                    info("Screenshot created");
                    blank_im = false;
                    break;
                case SDLK_n:
                    info("Screenshot discarded");
                    blank_im = false;
                    break;
                default:
                    break;
            }
        }
    }
    return end;
}

void handle_menu(char c)
{
    switch (c) {
        case SDLK_c:
            change_c = true;
            change_menu = false;
            break;
        case SDLK_r:
            change_r = true;
            change_menu = false;
        case SDLK_b:
            change_menu = false;
            info("Change menu left");
            break;
        default:
            // discard other inputs
            break;
    }
}
