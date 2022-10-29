#include <Arduino.h>
#include <toconfig.h>


// setting PWM properties
const int freq = 20000;
const int ledChannel = 0;
const int resolution = 8;


void toIO_Init(void)
{
    pinMode(LED_PIN,          OUTPUT);
    pinMode(OUTPUT_PIN,       OUTPUT);
    pinMode(BUTTON_PIN,       INPUT_PULLUP);
    pinMode(CURRENT_PIN,      INPUT);
    pinMode(OVER_CURRENT_PIN, INPUT);
    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(OUTPUT_PIN, ledChannel);
    Serial.begin(115200);
    Serial.println("BLOOM Start");
    Serial.print("name:    "); Serial.println(DEFAULT_DEVICE_NAME);
    Serial.print("version: "); Serial.println(DEFAULT_DEVICE_VERSION);
}