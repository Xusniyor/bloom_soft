#ifndef __TOCONFIG_H_
#define __TOCONFIG_H_

// qurulma oyoqchalari
#define LED_PIN          17 // LED chiroqcha
#define OUTPUT_PIN       16 // mattor ga PWM signal
#define BUTTON_PIN       18 // tugmacha
#define CURRENT_PIN      34 // mattor kuch
#define OVER_CURRENT_PIN 27 // mattor ortiqcha kuch
#define VCC_VOLTAGE_PIN  35 // tarmoq kuchlanishi
/*
// qurulma oyoqchalari
//#define LED_PIN          17 // LED chiroqcha
//#define OUTPUT_PIN       16 // mattor ga PWM signal
//#define BUTTON_PIN       34 // tugmacha
//#define CURRENT_PIN      26 // mattor kuch
//#define OVER_CURRENT_PIN 27 // mattor ortiqcha kuch
//#define VCC_VOLTAGE_PIN  35 // tarmoq kuchlanishi
*/
// qurulma xaqida
#define DEFAULT_DEVICE_NAME    "BLOOM\0"
#define DEFAULT_DEVICE_VERSION "1.0.1.5\0"
// access point mode: stations can connect to the ESP32
#define DEFAULT_AP_SSID        "BLOOM\0"
#define DEFAULT_AP_PASSWORD    "00000000\0"
// station mode: the ESP32 connects to an access point
#define DEFAULT_STA_SSID       "Xusniyor\0"
#define DEFAULT_STA_PASSWORD   "00000000\0"
// login parol
#define DEFAULT_LOGIN          "admin\0"
#define DEFAULT_PASSWORD       "admin\0"
// proshifka qilingan vaqti
#define DEFAULT_BUILD_DATA     "30.09.2021\0"
#define DEFAULT_BUILD_TIME     "14:54\0"
// yangilash uchun kalit
#define DEFAULT_UPDATE_URL     " \0"
#define DEFAULT_UPDATE_KEY     "07e5e493-c3e2-4af2-a15f-2afce5140557\0"

#endif /*__TOCONFIG_H_*/
