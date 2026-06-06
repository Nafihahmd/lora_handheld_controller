#include "app.h"
#include <HardwareSerial.h>

HardwareSerial Serial(PA10, PA9);

QueueHandle_t buttonQueue;
LedState ledStates[6];