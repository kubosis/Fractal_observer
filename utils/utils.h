#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define ANSI_INFO  "\033[0;36m\033[K"
#define ANSI_WARN  "\033[0;93m\033[K"
#define ANSI_DEBUG "\033[0;33m\033[K"
#define ANSI_ERROR "\033[0;31m\033[K"
#define ANSI_HELP  "\033[0;32m\033[K"
#define ANSI_RESET "\033[m\033[K"

#define TERMINAL_RAW 0
#define TERMINAL_RESET 1

/* allocate mem safely */
void *allocate_mem(size_t size);
void *realloc_mem(size_t size, void *ptr);

/* output with INFO level */
void info(char *str);

/* output with ERROR level */
void error(char *str);

/* output with WARNING level */
void warning(char *str);

/* output with DEBUG level */
void debug(char *str);

/* output with HELP level */
void help(char *str);

/* called with h or when run with -h */
void print_help();

/* called when run with -h */
void print_help_start();

/* sets terminal to raw or normal mode */
void set_raw(int reset);

/* help user with change options */
void print_change_info(bool change_r);

#endif
