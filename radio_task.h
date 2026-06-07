#pragma once
#include <Arduino.h>
// SPI1 pins
static const uint8_t LORA_SCK  = PB3;
static const uint8_t LORA_MISO = PB4;
static const uint8_t LORA_MOSI = PB5;

// SX1262 radio = new Module(LORA_NSS, LORA_DIO1, LORA_RST, LORA_BUSY);
static const uint8_t PIN_NSS  = PB0;
static const uint8_t PIN_BUSY = PB1;
static const uint8_t PIN_RST  = PB7;
static const uint8_t PIN_DIO1 = PA0;

void radioTask(void *pv);