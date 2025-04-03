#include <stdint.h>
#include <stdbool.h>

#include "ps2_keyboard.h"
#include "vga.h"
#include "idt.h"
#include "pic.h"

// Commands

// IRQ1
#define PS2_KBD_IRQ 0x01

#define CMD_KEY_SET_LED 0xed
#define CMD_KEY_ECHO 0xee

/*
Get or set the current scan code set.

Data byte used to indicate sub-command:
0   Get current scan code set
1   Set scan code set 1
2   Set scan code set 2
3   Set scan code set 3

Response:

0xfa (ACK) or 0xfe (resend) if scan code is being set
0xfa (ack), then scan code set number, or 0xfe (resend) if getting scan code
*/
#define CMD_SET_SCAN_CODE 0xf0
#define CMD_GET_SCAN_CODE CMD_SET_SCAN_CODE

#define CMD_KEY_IDENTIFY 0xf2
#define CMD_KEY_SET_TYPEMATIC_RATE 0xf3
#define CMD_KEY_ENABLE_SCAN 0xf4
#define CMD_KEY_DISABLE_SCAN 0xf5
#define CMD_KEY_SET_DEFAULT_PARAMS 0xf6
#define CMD_KEY_SET_ALL_TYPEMATIC 0xf7
#define CMD_KEY_SET_ALL_MAKE_RELEASE 0xf8
#define CMD_KEY_SET_ALL_MAKE 0xf9
#define CMD_KEY_SET_ALL_TYPEMATIC_AUTOREPEAT_MAKE_RELEASE 0xfa
#define CMD_KEY_SET_TYPEMATIC_AUTOREPEAT 0xfb
#define CMD_KEY_SET_MAKE_RELEASE 0xfc
#define CMD_KEY_SET_MAKE 0xfd
#define CMD_KEY_RESEND 0xfe
#define CMD_KEY_RESET 0xff

#define SCAN_CODE_SET_1 1
#define SCAN_CODE_SET_2 2
#define SCAN_CODE_SET_3 3

typedef enum {
    SET_1,
    SET_2,
    SET_3
} scan_code_set_t;

// Responses

// Key detection error or internal buffer overrun.
#define RESP_INTERNAL_ERROR 0x00

// Self test passed (sent after "0xFF (reset)" command or keyboard power up).
#define RESP_SELF_TEST_PASSED 0xaa

// Response to "0xEE (echo)" command.
#define RESP_ECHO 0xee

// Command acknowledged (ACK).
#define RESP_ACK 0xfa

// Self test failed (sent after "0xFF (reset)" command or keyboard power up).
#define RESP_SELF_TEST_FAILED 0xfc

// Self test failed (sent after "0xFF (reset)" command or keyboard power up).
#define RESP_SELF_TEST_FAILED2 0xfd

// Resend (keyboard wants controller to repeat last command it sent).
#define RESP_RESEND 0xfe

static uint8_t _vector = 0;
static uint8_t _port = 0;

// The interrupt vector used by the controller.
// static uint8_t interrupt_vector = 0x00;

/*
Get the interrupt vector corresponding to IRQ1. For now, we use 0x21.
*/
uint8_t get_vector() {
    // TODO: ask the PIC in case we've done some remapping?
    return pic_get_vector(PS2_KBD_IRQ);
}

// static bool is_responsive() {
//     write_byte(PORT_PS2_DATA, CMD_KEY_ECHO);
//     uint8_t resp = read_byte(PORT_PS2_DATA);
//     return resp == RESP_ECHO;
// }

void ps2_kbd_irq_handler() {
    println("ps2_kbd: Keyboard interrupt received");
}

static uint8_t get_scan_code_set_id(scan_code_set_t set) {
    switch (set) {
        case SET_1:
            return SCAN_CODE_SET_1;
        case SET_2:
            return SCAN_CODE_SET_2;
        case SET_3:
        default:
            return SCAN_CODE_SET_3;
    }
}

static scan_code_set_t get_scan_code_set(uint8_t id) {
    switch (id) {
        case SCAN_CODE_SET_3:
            return SET_3;
        case SCAN_CODE_SET_2:
            return SET_2;
        case SCAN_CODE_SET_1:
        default:
            return SET_1;
    }
}

static void ps2_kbd_send_cmd(uint8_t cmd) {
    ps2_send_device_command(cmd, _port);
}

static scan_code_set_t ps2_kbd_get_scan_code() {
    ps2_kbd_send_cmd(CMD_GET_SCAN_CODE);
    uint8_t id = ps2_read_data();
    return get_scan_code_set(id);
}

static void ps2_kbd_set_scan_codes(scan_code_set_t set) {
    uint8_t set_id = get_scan_code_set_id(set);
    ps2_kbd_send_cmd(CMD_SET_SCAN_CODE);
    ps2_send_data(set_id);

    // Check the scan code set.
    scan_code_set_t new_set = ps2_kbd_get_scan_code();
    if (new_set != set) {
        println("ps2_kbd_get_scan_code() gives scan code %d but expected %d...", get_scan_code_set_id(new_set), set_id);
        ps2_kbd_set_scan_codes(set);
    }
}

void ps2_kbd_init(port_t port) {
    // Ensure driver is not already initialised.
    if (_vector) return;

    _port = port;
    _vector = get_vector();

    // Install IRQ handler.
    uint16_t portno = port ? 1 : 0;
    println("Initialising PS/2 keyboard in port %d...", portno);

    // Unmask IRQ1.
    println("Unmasking IRQ1...");
    irq_clear_mask(0x01);

    // Enable scanning.
    println("Enabling scanning...");
    ps2_kbd_send_cmd(CMD_KEY_ENABLE_SCAN);

    // Set scan codes.
    // ps2_kbd_set_scancodes(SCANCODE_SET_2);
    println("Enabling scan code 1...");
    ps2_kbd_set_scan_codes(SET_1);
    // ps2_kbd_reset_state();


    // Install interrupt handler.
    println("Installing IRQ handler...");
    idt_install_irq_handler(_vector, ps2_kbd_irq_handler);

    println("Triggerring interrupt 0x21...");
    asm volatile("int $0x21");
    println("Successfully triggered interrupt 0x21!");
}
