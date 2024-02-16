#include <stdint.h>

#include "ps2.h"
#include "low_level.h"

// Commands

#define CMD_SET_LED 0xed
#define CMD_ECHO 0xee
#define CMD_SCAN_CODE 0xf0
#define CMD_IDENTIFY 0xf2
#define CMD_SET_TYPEMATIC_RATE 0xf3
#define CMD_ENABLE_SCAN 0xf4
#define CMD_DISABLE_SCAN 0xf5
#define CMD_SET_DEFAULT_PARAMS 0xf6
#define CMD_SET_ALL_TYPEMATIC 0xf7
#define CMD_SET_ALL_MAKE_RELEASE 0xf8
#define CMD_SET_ALL_MAKE 0xf9
#define CMD_SET_ALL_TYPEMATIC_AUTOREPEAT_MAKE_RELEASE 0xfa
#define CMD_SET_TYPEMATIC_AUTOREPEAT 0xfb
#define CMD_SET_MAKE_RELEASE 0xfc
#define CMD_SET_MAKE 0xfd
#define CMD_RESEND 0xfe
#define CMD_RESET 0xff

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

// Key detection error or internal buffer overrun.
// #define RESP_INTERNAL_ERROR2

void ps2_init() {

}
