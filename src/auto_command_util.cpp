/**
 * @file auto_command_util.cpp
 * @author interimadd
 * @brief よく使うSwitchへの入力をまとめたライブラリ
 * @version 0.1
 * @date 2020-01-14
 */
#include "auto_command_util.h"

// ボタンを押してから離すまでの時間
const uint16_t BUTTON_PUSHING_MSEC = 50;

// LEDに使うピン
const uint16_t LED_PIN = 13;

// 長押し時のLED点滅時間間隔
const uint16_t LED_INTERVAL = 500;

/**
 * @brief 初期化(13ピンをOUTPUTに設定)
 */
void initAutoCommandUtil()
{
    pinMode(LED_PIN, OUTPUT);
}

/**
 * @brief Switchコントローラーのボタンを押す
 * 
 * @param button 押すボタン
 * @param delay_after_pushing_msec ボタンを押し終えた後の待ち時間 
 * @param loop_num ボタンを押す回数
 */
void pushButton(Button button, int delay_after_pushing_msec, int loop_num)
{
    for(int i=0; i<loop_num; i++)
    {
        digitalWrite(LED_PIN, HIGH);
        SwitchController().pressButton(button);
        delay(BUTTON_PUSHING_MSEC);
        SwitchController().releaseButton(button);
        digitalWrite(LED_PIN, LOW);
        delay(delay_after_pushing_msec);
    }
    delay(BUTTON_PUSHING_MSEC);
}

/**
 * @brief Switchコントローラーの矢印ボタンを押す
 * 
 * @param button 押す矢印ボタン
 * @param delay_after_pushing_msec ボタンを押し終えた後の待ち時間 
 * @param loop_num ボタンを押す回数
 */
void pushHatButton(Hat button, int delay_after_pushing_msec, int loop_num)
{
    for(int i=0;i<loop_num;i++)
    {
        digitalWrite(LED_PIN, HIGH);
        SwitchController().pressHatButton(button);
        delay(BUTTON_PUSHING_MSEC);
        SwitchController().releaseHatButton();
        digitalWrite(LED_PIN, LOW);
        delay(delay_after_pushing_msec);
    }
    delay(BUTTON_PUSHING_MSEC);
}

/**
 * @brief Switchコントローラーの矢印ボタンを指定時間の間押し続ける
 * 
 * @param button 押す矢印ボタン
 * @param pushing_time_msec ボタンを押す時間の長さ
 */
void pushHatButtonContinuous(Hat button, int pushing_time_msec)
{
    SwitchController().pressHatButton(button);
    int remaining_time_msec = pushing_time_msec;
    while(true)
    {
        digitalWrite(LED_PIN, HIGH);
        if(remaining_time_msec > LED_INTERVAL){
            remaining_time_msec -= LED_INTERVAL;
            delay(LED_INTERVAL);
        }else{
            delay(remaining_time_msec);
            break;
        }
        digitalWrite(LED_PIN, LOW);
        if(remaining_time_msec > LED_INTERVAL){
            remaining_time_msec -= LED_INTERVAL;
            delay(LED_INTERVAL);
        }else{
            delay(remaining_time_msec);
            break;
        }
    }
    SwitchController().releaseHatButton();
    digitalWrite(LED_PIN, LOW); 
    delay(BUTTON_PUSHING_MSEC);
}

/**
 * @brief Switchのジョイスティックの倒し量を設定する
 * 
 * @param lx_per LスティックのX方向倒し量[％] -100~100の範囲で設定
 * @param ly_per LスティックのY方向倒し量[％] -100~100の範囲で設定
 * @param rx_per RスティックのX方向倒し量[％] -100~100の範囲で設定
 * @param ry_per RスティックのY方向倒し量[％] -100~100の範囲で設定
 * @param tilt_time_msec スティックを倒し続ける時間
 */
void tiltJoystick(int lx_per, int ly_per, int rx_per, int ry_per, int tilt_time_msec)
{
    SwitchController().setStickTiltRatio(lx_per, ly_per, rx_per, ry_per);
    int remaining_time_msec = tilt_time_msec;
    while(true)
    {
        digitalWrite(LED_PIN, HIGH);
        if(remaining_time_msec > LED_INTERVAL){
            remaining_time_msec -= LED_INTERVAL;
            delay(LED_INTERVAL);
        }else{
            delay(remaining_time_msec);
            break;
        }
        digitalWrite(LED_PIN, LOW);
        if(remaining_time_msec > LED_INTERVAL){
            remaining_time_msec -= LED_INTERVAL;
            delay(LED_INTERVAL);
        }else{
            delay(remaining_time_msec);
            break;
        }
    }
    SwitchController().setStickTiltRatio(0, 0, 0, 0);
    digitalWrite(LED_PIN, LOW); 
    delay(BUTTON_PUSHING_MSEC);
}
