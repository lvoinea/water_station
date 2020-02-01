/**
 *  This file initializes all drivers and system-wide logical
 *  resources/controllers that will be used during processing.
 *  
 *  The initialization is done per hardware unit or logical
 *  resource.
 */

//---------------------  Real Time Clock (DS1302)

// Set time for DS1302RTC
// In order to set-up the time the chip has to be suplied with 5V.
// However, reading the time is in that case not stable.
// In order to get stable read, one needs to suply the chip with 3.3V.

#include <ThreeWire.h>  
#include <RtcDS1302.h>
#include "rtc.h"

ThreeWire myWire(pinRtcDat, pinRtcClk, pinRtcRst); 
RtcDS1302<ThreeWire> Rtc(myWire);

//-------------------- Pomp Driver
#include "pomp_driver.h"

// number of miliseconds to wait for the water to stop dripping
int water_stop_time;

//-------------------- Arm Motor (28BYJ-48 + ULN2003)
#include <AccelStepper.h>
#include <MultiStepper.h>

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper cArmMotor(AccelStepper::HALF4WIRE, pinArmMotor1, pinArmMotor3, pinArmMotor2, pinArmMotor4);

//-------------------- Speaker
#include "pitches.h"

//-------------------- Power saver
#include <avr/sleep.h>  
#include <avr/wdt.h>
#include <avr/power.h>

//-------------------- Goals
#include "GoalManager.h"

GoalManager goal_manager;
Goal current_goal;
