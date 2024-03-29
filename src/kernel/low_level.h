#ifndef _DREWOS_LOW_LEVEL_H_
#define _DREWOS_LOW_LEVEL_H_

/*
Read a single byte from the specified port.

@param port: The port from which to read.
*/
unsigned char read_byte(unsigned short port);

/*
Write a single byte to the specified port.

@param port: The port to which data will be written.
@param data: The byte to be written.
*/
void write_byte(unsigned short port, unsigned char data);

/*
Read a word from the specified port.

@param port: The port from which to read.
*/
unsigned short read_word(unsigned short port);

/*
Write a word to the specified port.

@param port: The port to which data will be written.
@param data: The word to be written.
*/
void write_word(unsigned short port, unsigned short data);

/*
Wait a very small amount of time (usually 1-4ms). Useful for implementing a
small delay for PIC remapping on old hardware, or generally as a simple but
imprecise wait.
*/
void io_wait();

#endif // _DREWOS_LOW_LEVEL_H_
