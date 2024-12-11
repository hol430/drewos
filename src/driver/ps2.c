#include <stdint.h>
#include <stdbool.h>

#include "ps2.h"
#include "low_level.h"
#include "vga.h"
#include "util.h"

// Driver for a 8042 PS/2 Controller

// Commands

// Read byte N from internal RAM where N is the command byte & 0x1f.
// 0x20 reads byte 0, 0x21 - 0x3f reads byte 1 - 31.
#define CMD_READ_RAM 0x20

// Write the next byte to byte N of internal RAM, where N is the command byte
// & 0x1f.
#define CMD_WRITE_RAM 0x60

#define CMD_DISABLE_PORT_1 0xa7
#define CMD_ENABLE_PORT_1 0xa8

// Test second PS/2 port.
#define CMD_TEST_PORT_1 0xa9

// Test PS/2 controller. Response 0x55 = test passed, 0xfc = test failed.
#define CMD_TEST 0xaa

#define CMD_TEST_PORT_0 0xab
#define CMD_DUMP 0xac
#define CMD_DISABLE_PORT_0 0xad
#define CMD_ENABLE_PORT_0 0xae
#define CMD_READ_INPUT 0xc0
#define CMD_COPY_INPUT_LOWER 0xc1
#define CMD_COPY_INPUT_UPPER 0xc2
#define CMD_READ_OUTPUT 0xd0
#define CMD_WRITE_OUTPUT 0xd1
#define CMD_WRITE_OUTPUT_PORT_0 0xd2
#define CMD_WRITE_OUTPUT_PORT_1 0xd3
#define CMD_WRITE_INPUT_PORT_1 0xd4

#define CMD_PULSE_OUTPUT_LINE 0xf0

#define CMD_RESET_DEVICE 0xff

// Key detection error or internal buffer overrun.
// #define RESP_INTERNAL_ERROR2

// Ports

// 8042 PS/2 data read/write port.
#define PORT_DATA 0x60

// 8042 PS/2 status register.
#define PORT_STATUS 0x64

// 8042 PS/2 command register. This is used for sending commands to the PS/2
// controller (not to PS/2 devices).
#define PORT_COMMAND 0x64

// Other constants.

// Default max number of iterations to wait for the device to become ready.
#define DEFAULT_WAIT 1024

#define RESP_RESET_PASS0 0xfa
#define RESP_RESET_PASS1 0xaa
#define RESP_RESET_FAIL 0xfc

// Data types.

typedef struct {
    // Output buffer status (0 = empty, 1 = full).
    bool outb_full : 1;

    // Input buffer status (0 = empty, 1 = full).
    bool inb_full : 1;

    // Cleared on reset and set by firmware via config byte if system passes
    // self tests.
    bool system_flag : 1;

    // 0 = data written to input buffer is for PS/2 device.
    // 1 = data written to input buffer is for PS/2 controller.
    bool command_data : 1;

    // Unknown/unused. May be "keyboard lock". Chipset-specific.
    bool keyboard_lock : 1;

    // Unknown (chipset-specific). May be "receive timeout" or "second PS/2 port
    // output buffer full".
    bool receive_timeout : 1;

    // Timeout error (0 = no error, 1 = timeout error).
    bool timeout_error : 1;

    // Parity error (0 = no error, 1 = parity error).
    bool parity_error : 1;
} __attribute__((packed)) status_t;

typedef struct {
    // First PS/2 port interrupt (1 = enabled, 0 = disabled).
    bool port_0_interrupt : 1;

    // Second PS/2 port interrupt (1 = enabled, 0 = disabled). Only used if 2
    // PS/2 ports are supported.
    bool port_1_interrupt : 1;

    // System flag (1 = system passed POST, 0 = bad news).
    bool system_flag : 1;

    // Should be zero.
    bool unused : 1;
    bool clock_0 : 1;
    bool clock_1 : 1;
    bool port_0_translation : 1;
    bool zero : 1;
} __attribute__((packed)) config_t;

typedef union {
    uint8_t byte;
    config_t config;
} config_resp_t;

// Colours usable in VGA colour text mode.
typedef enum {
    // Ancient AT keyboard.
    KBD_AT,
    // Standard PS/2 mouse.
    MOUSE_PS2,
    // Mouse with scroll wheel.
    MOUSE_SCROLL_WHEEL,
    // 5-button mouse.
    MOUSE_5BUTTON,
    // MF2 keyboard.
    KBD_MF2,
    // IBM ThinkPads, Spacesaver keyboards, many other "short" keyboards.
    KBD_SHORT,
    // NCD N-97 keyboard, or 122-Key host connect(ed) keyboard.
    KBD_N97,
    // 122-key keyboards.
    KBD_122KEY,
    // Japanese "G" keyboards.
    KBD_JPN_G,
    // Japanese "P" keyboards.
    KBD_JPN_P,
    // Japanese "A" keyboards.
    KBD_JPN_A,
    // NCD Sun layout keyboard.
    KBD_NCD_SUN
} device_type_t;

static bool port_0_operational = false;
static bool port_1_operational = false;
static bool dual_channel = false;

// static device_type_t port_0_type;
// static device_type_t port_1_type;

static uint8_t get_status() {
    return read_byte(PORT_STATUS);
}

// Check if the controller is ready to receive a command. This occurs when bit 1
// of the status register is clear.
static bool command_ready() {
    return (get_status() & 0x02) == 0;
}

/*
Check if the controller has written a response byte. This occurs when bit 0 of
the status register is set.
*/
static bool response_ready() {
    return get_status() & 0x01;
}

/*
Wait until the controller is ready for a command. This occurs when bit 1 of the
status register is clear. Return true when the controller is ready, or false
if wait iterations elapse.

@param wait: Maximum number of iterations to wait.
*/
static bool buffer_wait() {
    uint32_t wait = DEFAULT_WAIT;
    while (!command_ready() && wait--);
    return wait;
}

static bool response_wait() {
    uint32_t wait = DEFAULT_WAIT;
    while (!response_ready() && wait--);
    return wait;
}

static void send_command(uint8_t command) {
    if (!buffer_wait()) {
        println("Error: ps/2 controller failed to enter ready state");
        return;
    }

    write_byte(PORT_COMMAND, command);
}

static void send_data(uint8_t data) {
    if (!buffer_wait()) {
        println("Error: PS/2 controller faield to enter ready state");
        return;
    }
    write_byte(PORT_DATA, data);
}

static bool try_read_data(uint8_t *data) {
    if (response_wait()) {
        *data = read_byte(PORT_DATA);
        return true;
    }
    return false;
}

static uint8_t read_data() {
    uint8_t byte;
    bool resp = try_read_data(&byte);
    if (!resp) {
        println("Error: PS/2 controller did not write a response");
        return 0;
    }
    return byte;
}

static config_resp_t get_config() {
    send_command(CMD_READ_RAM);

    config_resp_t config;
    config.byte = read_data();

    return config;
}

static void set_config(config_resp_t config) {
    send_command(CMD_WRITE_RAM);
    send_data(config.byte);
}

static void print_port_error_msg(uint8_t status) {
    if (status == 0x01) {
        println("Clock line stuck low");
    } else if (status == 0x02) {
        println("Clock line stuck high");
    } else if (status == 0x03) {
        println("Data line stuck low");
    } else if (status == 0x04) {
        println("Data line stuck high ");
    } else {
        println("Unknown error");
    }
}

static void reset_port_0() {
    send_data(CMD_RESET_DEVICE);
    uint8_t resp[4];
    bool has_byte = try_read_data(&resp[0]);
    if (!has_byte) {
        println("Port 0 is not populated");
        return;
    }
    has_byte = try_read_data(&resp[1]);
    if (!has_byte) {
        println("Port 0 is not populated");
        return;
    }
    has_byte = try_read_data(&resp[2]);
    if (!has_byte) {
        println("Port 0 is not populated");
        return;
    }
    has_byte = try_read_data(&resp[3]);
    if (!has_byte) {
        println("Port 0 is not populated");
        return;
    }

    if (resp[0] == RESP_RESET_FAIL) {
        println("PS/2 port 0 reset failed");
        return;
    }

    if ( (resp[0] == RESP_RESET_PASS0 && resp[1] == RESP_RESET_PASS1) ||
         (resp[0] == RESP_RESET_PASS1 && resp[1] == RESP_RESET_PASS0) ) {
        println("Successfully reset port 0");
    }

    // TODO: device ID detection.
    int16_t device_id = (resp[2] << 8) + resp[3];
    char buf[32];
    itoh(device_id, buf, 32);
    print("Port 0 device ID: ");
    println(buf);
}

void ps2_init() {
    // todo: Disable USB legacy support

    // Disable devices to avoid them sending data partway through initialisation
    // of the controller.
    send_command(CMD_DISABLE_PORT_0);
    send_command(CMD_DISABLE_PORT_1);

    // Flush output buffer.
    read_byte(PORT_DATA);

    // Modify the controller configuration.

    // Get the current configuration.
    config_resp_t config = get_config();

    // Disable IRQs and translation for port 1 by clearing bits 0 and 6. Also
    // ensure the clock signal is enabled by clearing bit 4.
    config.config.port_0_interrupt = false;
    config.config.port_0_translation = false;
    config.config.clock_0 = false;

    // Now write the updated config.
    set_config(config);

    // To test the PS/2 controller, send command 0xAA to it, then wait for it to
    // respond and check if it replied with 0x55 (any value other than 0x55
    // indicates a self-test fail).
    send_command(CMD_TEST);

    uint8_t result = read_data();
    if (result != 0x55) {
        println("Error: PS/2 self-test failed");
        return;
    }

    // The self-test can reset some controllers. In such cases, the config byte
    // should be restored to its previous value.
    config_resp_t new_config = get_config();
    if (new_config.byte != config.byte) {
        set_config(config);
    }

    // Check if this is a dual-channel controller.

    // Enable the second PS/2 port and read the config byte (command 0x20). Bit
    // 5 of the Controller Configuration Byte should be clear - if it's set then
    // it can't be a dual channel PS/2 controller, because the second PS/2 port
    // should be enabled.
    send_command(CMD_ENABLE_PORT_1);

    // If a second port exists, enabling that port should cause the clock1 bit
    // of the config byte to be clear.
    config = get_config();
    dual_channel = config.config.clock_1 == 0;

    // If it's a dual channel device, disable the second port and then disable
    // IRQs and enable the clock for port 2.
    if (dual_channel) {
        send_command(CMD_DISABLE_PORT_1);
        config.config.port_1_interrupt = false;
        config.config.clock_1 = false;
        set_config(config);
    }

    // This step tests the PS/2 ports. Use command 0xAB to test the first PS/2
    // port, then check the result. Then (if it's a dual channel controller) use
    // command 0xA9 to test the second PS/2 port, then check the result.
    send_command(CMD_TEST_PORT_0);
    uint8_t resp = read_data();

    port_0_operational = resp == 0;
    if (!port_0_operational) {
        print("PS/2 port 0 is faulty: ");
        print_port_error_msg(resp);
    }

    if (dual_channel) {
        send_command(CMD_TEST_PORT_1);
        resp = read_data();
        port_1_operational = resp == 0;
        if (!port_1_operational) {
            print("PS/2 port 1 is faulty: ");
            print_port_error_msg(resp);
        }
    }

    if (!port_0_operational && (!dual_channel || !port_1_operational) ) {
        println("No PS/2 ports are operational. PS/2 driver will be disabled.");
        return;
    }

    // Enable functional devices.
    if (port_0_operational) {
        send_command(CMD_ENABLE_PORT_0);
    }
    if (port_1_operational) {
        send_command(CMD_ENABLE_PORT_1);
    }

    // If using interrupts, enable interrupts on all functional ports.
    // config = get_config();
    // if (port_0_operational) {
    //     config.config.port_0_interrupt = true;
    // }
    // if (port_1_operational) {
    //     config.config.port_1_interrupt = true;
    // }
    // set_config(config);

    // All devices should be reset by sending 0xFF to port 1 (and port 2 for
    // dual channel controllers) and waiting for a response. If the response is
    // 0xFA, 0xAA (Note: the order in which devices send these two seems to be
    // ambiguous) followed by the device PS/2 ID, then the device reset was
    // successful. If the response is 0xFC then the self test failed and the
    // device should be ignored. If there's no response at all then the port is
    // not populated. 

    if (port_0_operational) {
        reset_port_0();
    }

    // To send data to the first PS/2 Port:

    // Set up some timer or counter to use as a time-out
    // Poll bit 1 of the Status Register ("Input buffer empty/full") until it becomes clear, or until your time-out expires
    // If the time-out expired, return an error
    // Otherwise, write the data to the Data Port (IO port 0x60)
}
