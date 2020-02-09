
#include "config.h"
#include "resources.h"
#include "setup.h"
#include "loop.h"

/**
 * Required compile time configuration:
 * 
 * - NUMBER_OF_BEEPS     : number of warning beeps during initialization to clear the area
 * - CHECK_TIME_INTERVAL : number of 8 second intervals before checking the wake-up condition
 */

/**
 * TODO:
 * - Read configuration settings from serial.
 * - Initialize the cylinder register from EPROM.
 * - Test with RTC in circuit.
 * - Migrate the circuit to a standalone chip. (3V required by the RTC to read correctly).
 * - Add amplification circuit to buzzer.
 * 
 *  DONE:
 * - Add a buzzer to signal start.
 * - Add error condition led.
 * - Add a wake function based on a trigger from the RTC or a sleep (?).
 * - Add a way to measure power consumption.
 * - Add demo mode button.
 * - Fix serial display.
 * 
 */
