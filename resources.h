/**
 *  This file initializes all drivers and system-wide logical
 *  resources/controllers that will be used during processing.
 *  
 *  The initialization is done per hardware unit or logical
 *  resource.
 */

//---------------------  Real Time Clock (DS3231)
#include <DS3231.h>
#include "rtc.h"

DS3231  rtc(pinSDA, pinSCL);

//-------------------- Pomp Driver
#include "pomp_driver.h"

//-------------------- Arm Motor (28BYJ-48 + ULN2003)
#include <AccelStepper.h>
#include <MultiStepper.h>

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper cArmMotor(AccelStepper::HALF4WIRE, pinArmMotor1, pinArmMotor3, pinArmMotor2, pinArmMotor4);

//-------------------- Speaker
#include "speaker.h"

//-------------------- Power saver
#include <avr/sleep.h>  
#include <avr/wdt.h>
#include <avr/power.h>

//-------------------- EEPROM
#include <EEPROMex.h>

int reserve_int_eeprom(){
  return(EEPROM.getAddress(sizeof(int)));
}

int reserve_byte_eeprom(){
  return(EEPROM.getAddress(sizeof(byte)));
}

//-------------------- Cylinders
#include "CylinderRegister.h"

CylinderRegister cylinder_register;
Cylinder current_cylinder;

//-------------------- Timers
#include "TimerRegister.h"

TimerRegister timer_register;
