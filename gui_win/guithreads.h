/* 
 * File name: guithreads.h
 * Description: 
 * header for guithreads.c
 * Date: 05/05/2021
 * Author: Jakub Sukdol
 */

#ifndef __GUITHREADS_H__
#define __GUITHREADS_H__

/* condition for animation */
//pthread_cond_t cnd;

/* animate until not stopped */
void *animation_thread(void *v);


#endif
