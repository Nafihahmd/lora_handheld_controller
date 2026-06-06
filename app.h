#pragma once

#include <STM32FreeRTOS.h>
#include <HardwareSerial.h>

extern HardwareSerial Serial;

#define LOGI(fmt, ...) \
    Serial.printf("[I] " fmt "\r\n", ##__VA_ARGS__)

#define LOGE(fmt, ...) \
    Serial.printf("[E] " fmt "\r\n", ##__VA_ARGS__)

#define NUM_BUTTONS 6
#define NUM_PIXELS  6

#define PIXEL_PIN   PA4
#define LED_CTRL    PA5
#define BUZZER_PIN  PA8

enum LedState {
    LED_OFF,
    LED_READY,
    LED_TRIGGERED,
    LED_WAITING,
    LED_ERROR
};

enum ButtonEventType {
    BTN_SHORT,
    BTN_LONG
};

struct ButtonEvent {
    uint8_t button;
    ButtonEventType type;
};

extern QueueHandle_t buttonQueue;
extern LedState ledStates[6];