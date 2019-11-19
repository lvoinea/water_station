
#include "config.h"
#include "resources.h"
#include "setup.h"
#include "loop.h"

/**
 * Required configuration:
 * 
 * - NUMBER_OF_BEEPS     : number of warning beeps during initialization to clear the area
 * - CHECK_TIME_INTERVAL : number of 8 second intervals before checking the wake-up condition
 * - goal_manager        : target configuration (position and duration of delivery)
 */

/**
 * TODO:
 * 
 * -. Add a buzzer to signal start
 * -. Add error condition led
 * -. Add a wake function based on a trigger from the RTC or a sleep (?)
 * 4. Add a way to measure power consumption.
 * 5. Migrate the circuit to a standalone chip. (3V required by the RTC to read correctly).
 * 6. Initialize the goal manager from a file on a SD or from serial.
 * 
 */
