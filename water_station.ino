
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
 * - Add distance
 * - Add amplification circuit to buzzer.
 * 
 *  DONE:
 * - Add a buzzer to signal start.
 * - Add error condition led.
 * - Add a wake function based on a trigger from the RTC or a sleep (?).
 * - Add a way to measure power consumption.
 * - Add demo mode button.
 * - Fix serial display.
 * - Test with RTC in circuit.
 * - Read configuration settings from serial.
 * - Initialize the cylinder register from EPROM.
 * 
 */

/**
 * RESOURCES:
 * - http://www.rinkydinkelectronics.com/library.php?id=73 - DS3231
 * - https://thijs.elenbaas.net/2012/07/extended-eeprom-library-for-arduino - EEPROMex
 * - https://www.baldengineer.com/arduino-internal-pull-up-resistor-tutorial.html - Pullup resistors
 * 
 */
