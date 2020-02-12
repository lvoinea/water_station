/**
 *  This file configures the MCU pin assignment
 *  and sets program wide constants.
 */


//--------------------- Serial

#define SERIAL_SPEED 9600

//--------------------- Control Panel

#define pinDemo A0
#define pinSettings A1

//---------------------  Real Time Clock (DS3231)
#define pinSDA 8
#define pinSCL 9

//-------------------- Cylinders
#define NR_CYLINDERS 6

//-------------------- Timers
#define NR_TIMERS 4

//-------------------- Pomp Driver
#define pinPompDriver 10

//-------------------- Zero position switch
#define pinZeroPos 6

//-------------------- Arm Motor (28BYJ-48 + ULN2003)
#define STEPS_PER_REV 4076 // Only with half step mode. For full step configuration use 2038

// Motor pin definitions
#define pinArmMotor1  2     // IN1 on the ULN2003 driver
#define pinArmMotor2  3     // IN2 on the ULN2003 driver
#define pinArmMotor3  4     // IN3 on the ULN2003 driver
#define pinArmMotor4  5     // IN4 on the ULN2003 driver

// Reference position for start (~ a bit more than a 360 degree turn)
// During calibration, motor will try to reach to this position, and stop when
// the zero position switch is triggered. 
#define ZERO_POSITION -(STEPS_PER_REV+100)

//-------------------- Speaker
#define pinSpeaker 13

#define NUMBER_OF_BEEPS 3    // number of start warning beeps upon initilaization

//-------------------- Display
#define pinError 12
#define pinOn 11

#define BLINK_ON 100
#define BLINK_OFF 2000

//-------------------- Power saver
#define CHECK_TIME_INTERVAL 1 // number of 8s cycles before checking wake-up condition
