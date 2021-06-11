/*
 * File name: prg_serial_nonblock.h
 * Date:      2017/04/07 22:33
 * Author:    Jan Faigl && Jakub Sukdol
 */

#ifndef __PRG_SERIAL_NONBLOCK_H__
#define __PRG_SERIAL_NONBLOCK_H__

#define BAUD_NORMAL B115200
#define RATE_NORMAL 0
#define BAUD_FAST1 B230400
#define RATE_FAST1 1
#define BAUD_FAST2 B460800
#define RATE_FAST2 2 
#define BAUD_FASTEST B576000
#define RATE_FASTEST 3
#define BAUD_NUM 4


/// ----------------------------------------------------------------------------
/// @brief serial_open
/// 
/// @param fname  -- device name
/// 
/// @return 
/// ----------------------------------------------------------------------------
int serial_open(const char *fname, int rate);

/// ----------------------------------------------------------------------------
/// @brief serial_close
/// 
/// @param fd 
/// 
/// @return 
/// ----------------------------------------------------------------------------
int serial_close(int fd);

/// ----------------------------------------------------------------------------
/// @brief serial_putc
/// 
/// @param fd 
/// @param c 
/// 
/// @return 
/// ----------------------------------------------------------------------------
int serial_putc(int fd, char c);

/// ----------------------------------------------------------------------------
/// @brief serial_getc
/// 
/// @param fd 
/// 
/// @return 
/// ----------------------------------------------------------------------------
int serial_getc(int fd);

/// ----------------------------------------------------------------------------
/// @brief serial_getc_timeout
/// 
/// @param fd 
/// 
/// @return -1 on error, 0 no byte ready within the timeout, 1 one byte read
/// ----------------------------------------------------------------------------
int serial_getc_timeout(int fd, int timeout_ms, unsigned char *c);

#endif

/* end of prg_serial_nonblock.h */
