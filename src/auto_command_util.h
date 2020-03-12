#include "switch_controller.h"

#pragma once

// LEDに使うピン
const uint16_t LED_RED_PIN = 9;
const uint16_t LED_GREEN_PIN = 6;
const uint16_t LED_BLUE_PIN = 5;
const uint16_t LED_WHITE_PIN = 3;
const uint16_t LED_RED_GND_PIN = 8;
const uint16_t LED_GREEN_GND_PIN = 7;
const uint16_t LED_BLUE_GND_PIN = 4;

// 明るさ
const uint16_t LED_BRIGHTNESS_100 = 31;
const uint16_t LED_BRIGHTNESS_50 = 15;

void initAutoCommandUtil();
void pushButton(Button button, int delay_after_pushing_msec, int loop_num=1);
void pushHatButton(Hat button, int delay_after_pushing_msec, int loop_num=1);
void pushHatButtonContinuous(Hat button, int pushing_time_msec);
void tiltJoystick(int lx_per, int ly_per, int rx_per, int ry_per, int tilt_time_msec);
void redLED(int brightness);
void whiteLED(int brightness);
void blueLED(int power);
void greenLED(int power);
