/*
 * chrisRGBUtils.h
 *
 *  Self-contained utilities for generating a configurable PWM signal and
 *  outputting on the Tiva-C launchpad RGB LED.
 *
 *  Created on: Feb 5, 2017
 *      Author: Christopher Green
 */

#ifndef CHRIS_RGB_UTILS_H_
#define CHRIS_RGB_UTILS_H_

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom_map.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"

// Parameters for fading
#define PERIOD_TICKS 2500                   // Number of system clock cycles in a PWM period (remember, 50MHz clock)
#define FADE_DELAY_US 250                   // how many microseconds between each fade increment
#define EXP_INCREMENT (float)0.003129       // For a 2500-point exponential fade, this is the increment value for the exponential

// Macro function definitions
#define delay_us(us) SysCtlDelay((SysCtlClockGet()/(3000000))*us)

// These functions can pulse primary and secondary colors...
// Linearly
#define pulseRed() pulse(setRed)
#define pulseGreen() pulse(setGreen)
#define pulseBlue() pulse(setBlue)
#define pulseYellow() pulsePair(setRed, setGreen)
#define pulseTeal() pulsePair(setBlue, setGreen)
#define pulsePurple() pulsePair(setRed, setBlue)
//Exponentially
#define expPulseRed() expPulse(setRed)
#define expPulseGreen() expPulse(setGreen)
#define expPulseBlue() expPulse(setBlue)
#define expPulseYellow() expPulsePair(setRed, setGreen)
#define expPulseTeal() expPulsePair(setBlue, setGreen)
#define expPulsePurple() expPulsePair(setRed, setBlue)

// I'm not including white here because it requires more computation to make the LED look white, launchpad resistors are not well-picked

void rgbInit(uint16_t);                             // Configures GPIO and PWM generator blocks, as well as number of ticks in a period
inline void rgbEnable();                            // Enable LED output
inline void rgbDisable();                           // Disable RGB output
void setGreen(uint16_t);                            // Set to a certain value from 0->period
void setBlue(uint16_t);                             // Set to a certain value from 0->period
void setRed(uint16_t);                              // Set to a certain value from 0->period
void pulse(void (*setDuty)(uint16_t));              // Use one of the 'setColor' functions to do a linear-fading pulse
void pulsePair(void (*setDuty1)(uint16_t), void (*setDuty2)(uint16_t));     // Same as above but fades as a pair for non-primary colors
void expPulse(void (*setDuty)(uint16_t));           // Use one of the 'setColor' functions to do an exponentially-fading pulse
void expPulsePair(void (*setDuty1)(uint16_t), void (*setDuty2)(uint16_t));    // Same as above but fades as a pair for non-primary colors

#endif /* CHRIS_RGB_UTILS_H_ */
