/*
 * chrisRGBUtils.c
 *
 *  Self-contained utilities for generating a configurable PWM signal and
 *  outputting on the Tiva-C launchpad RGB LED.
 *
 *  Created on: Feb 5, 2017
 *      Author: Christopher Green
 */

#include "chrisRGBUtils.h"

void rgbInit(uint16_t pwmPeriodTicks){ // Configures GPIO and PWM generator blocks, as well as number of ticks in a period
    // Initialize GPIO peripheral
    // Initialize and configure GPIO-F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    // Initialize GPIO peripheral
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {} // Wait for initialization

    // Configure GPIO pins for PWM
    GPIOPinConfigure(GPIO_PF3_M1PWM7); // Map GPIO PF3 (green LED) to PWM gen
    GPIOPinConfigure(GPIO_PF2_M1PWM6); // Map GPIO PF2 (blue LED) to PWM gen
    GPIOPinConfigure(GPIO_PF1_M1PWM5); // Map GPIO PF1 (red LED) to PWM gen
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1);

    // Initialize PWM Peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1); // Enables PWM1 generator
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM1)) {} // Wait for it to initialize

    // Configure PWM generator
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC); // Configure PWM1-GEN3 to operate in down-mode
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC); // Configure PWM1-GEN2 to operate in down-mode

    // This isn't really needed but I'm leaving it here to tweak
    SysCtlPWMClockSet(SYSCTL_PWMDIV_1); // PWM clock = system clock (special function for TM4C123)

    // Configure period duration
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, pwmPeriodTicks); // Green & Blue
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, pwmPeriodTicks); // Red

    // Set initial duty cycle for all three outputs (1 = almost off)
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, 1); // Green
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 1); // Blue
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 1); // Red

    // Enable PWM but don't turn on output yet
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
}

// This does exactly what you think it does...
inline void rgbEnable(){
    PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT|PWM_OUT_6_BIT|PWM_OUT_5_BIT, true);
}

// ...so does this
inline void rgbDisable(){
    PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT|PWM_OUT_6_BIT|PWM_OUT_5_BIT, false);
}

// These functions shouldn't be 'inline' because they may be called MANY times

// Set to a certain value from 0->period
void setGreen(uint16_t ticks){
    if(ticks != 0)
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ticks);
    else // Setting this value to zero causes incorrect behavior
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, 1);
}

// Set to a certain value from 0->period
void setBlue(uint16_t ticks){
    if(ticks != 0)
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ticks);
    else // Setting this value to zero causes incorrect behavior
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, 1);
}

// Set to a certain value from 0->period
void setRed(uint16_t ticks){
    if(ticks != 0)
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ticks);
    else // Setting this value to zero causes incorrect behavior
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, 1);
}

// Generic fade function that can call a function to set duty
void pulse(void (*setDuty)(uint16_t)){
    int i;
    // Fade on
    for(i = 0; i < PERIOD_TICKS; i++){
        setDuty(i);
        delay_us(FADE_DELAY_US);
    }
    // Fade off
    for(i = PERIOD_TICKS; i > 0; i--){
        setDuty(i);
        delay_us(FADE_DELAY_US);
    }
}

// Generic fade function that can call a function to set duty
void pulsePair(void (*setDuty1)(uint16_t), void (*setDuty2)(uint16_t)){
    int i;
    // Fade on
    for(i = 0; i < PERIOD_TICKS; i++){
        setDuty1(i);
        setDuty2(i);
        delay_us(FADE_DELAY_US);
    }
    // Fade off
    for(i = PERIOD_TICKS; i > 0; i--){
        setDuty1(i);
        setDuty2(i);
        delay_us(FADE_DELAY_US);
    }
}

// Generic fade function that can call a function to set duty for two colors simultaneously
void expPulse(void (*setDuty)(uint16_t)){
    int i;
    // Fade on
    for(i = 0; i < PERIOD_TICKS; i++){
        setDuty(floor(expf(i*EXP_INCREMENT)));
        delay_us(FADE_DELAY_US);
    }
    // Fade off
    for(i = PERIOD_TICKS; i > 0; i--){
        setDuty(floor(expf(i*EXP_INCREMENT)));
        delay_us(FADE_DELAY_US);
    }
}

// Generic fade function that can call a function to set duty for two colors simultaneously
void expPulsePair(void (*setDuty1)(uint16_t), void (*setDuty2)(uint16_t)){
    int i;
    // Fade on
    for(i = 0; i < PERIOD_TICKS; i++){
        setDuty1(floor(expf(i*EXP_INCREMENT)));
        setDuty2(floor(expf(i*EXP_INCREMENT)));
        delay_us(FADE_DELAY_US);
    }
    // Fade off
    for(i = PERIOD_TICKS; i > 0; i--){
        setDuty1(floor(expf(i*EXP_INCREMENT)));
        setDuty2(floor(expf(i*EXP_INCREMENT)));
        delay_us(FADE_DELAY_US);
    }
}
