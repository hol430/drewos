#ifndef _DREWOS_PS2_H_
#define _DREWOS_PS2_H_

#include <stdbool.h>
#include <stdint.h>

// The ports available on the device.
typedef enum {
    PORT0,
    PORT1
} port_t;

/*
Initialise the PS/2 controller. This should only be called if a PS/2 controller
exists (see ps2_controller_exists()).
*/
void ps2_init();

/*
Send a command to the controller.

@param command: The command.
*/
void ps2_send_command(uint8_t command);


/*
Send a command a ps2 device, and wait for ACK.

@param cmd: The command.
@param port: The port.
*/
void ps2_send_device_command(uint8_t cmd, port_t port);

/*
Wait until the controller is ready for a command. Return true when the
controller is ready, or false if wait iterations elapse.
*/
bool ps2_buffer_wait();

/*
Write the specified byte of data to the ps2 data port, after waiting for the
buffer to clear (indicating readiness).
*/
void ps2_send_data(uint8_t data);

/*
Read data from the data port.
*/
uint8_t ps2_read_data();

#endif // _DREWOS_PS2_H_
