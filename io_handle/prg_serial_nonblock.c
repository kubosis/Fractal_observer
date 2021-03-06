/*
 * File name: prg_serial_nonblock.c
 * Date:      2017/04/07 22:34
 * Author:    Jan Faigl && Jakub Sukdol
 *            aka Honza & Kubos
 */

#include <assert.h>

#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

#include <poll.h>

#include "prg_serial_nonblock.h"

/// ----------------------------------------------------------------------------
int serial_open(const char *fname, int rate)
{
   int fd = open(fname, O_RDWR | O_NOCTTY | O_SYNC);

   struct termios term;
   if(tcgetattr(fd, &term) >= 0) {
   cfmakeraw(&term);
   term.c_cc[VTIME] = 0; //set vtime 
   term.c_cc[VMIN] = 1;

   switch (rate) {
      case RATE_NORMAL:
         cfsetispeed(&term, BAUD_NORMAL);
         cfsetospeed(&term, BAUD_NORMAL);
         break;
      case RATE_FAST1:
         cfsetispeed(&term, BAUD_FAST1);
         cfsetospeed(&term, BAUD_FAST1);
         break;
      case RATE_FAST2:
         cfsetispeed(&term, BAUD_FAST2);
         cfsetospeed(&term, BAUD_FAST2);
         break;
      case RATE_FASTEST:
         cfsetispeed(&term, BAUD_FASTEST);
         cfsetospeed(&term, BAUD_FASTEST);
         break;
   }
   assert(tcsetattr(fd, TCSADRAIN, &term) >= 0);
   assert(fcntl(fd, F_GETFL) >= 0);
   assert(tcsetattr(fd, TCSADRAIN, &term) >= 0);
   assert(fcntl(fd, F_GETFL) >= 0);
   tcflush(fd, TCIFLUSH);
   tcsetattr(fd, TCSANOW, &term);

   // Set the serial port to non block mode
   int flags = fcntl(fd, F_GETFL);
   flags &= ~O_NONBLOCK;
   assert(fcntl(fd, F_SETFL, flags) >= 0);
   } else {
      fd = -1;
   }
   return fd;
}

/// ----------------------------------------------------------------------------
int serial_close(int fd)
{
   return close(fd);
}

/// ----------------------------------------------------------------------------
int serial_putc(int fd, char c)
{
   return write(fd, &c, 1);
}

/// ----------------------------------------------------------------------------
int serial_getc(int fd)
{
   char c;
   int r = read(fd, &c, 1);
   return r == 1 ? c : -1;
}

/// ----------------------------------------------------------------------------
int serial_getc_timeout(int fd, int timeout_ms, unsigned char *c)
{
   struct pollfd ufdr[1];
   int r = 0;
   ufdr[0].fd = fd;
   ufdr[0].events = POLLIN | POLLRDNORM;
   if ((poll(&ufdr[0], 1, timeout_ms) > 0) && (ufdr[0].revents & (POLLIN | POLLRDNORM))) {
      r = read(fd, c, 1);
   }
   return r;
}

/* end of prg_serial_nonblock.c */
