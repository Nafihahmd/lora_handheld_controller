#include "app.h"

#include "button_task.h"
#include "radio_task.h"
#include "led_task.h"

void setup()
{
    buttonQueue =
        xQueueCreate(
            10,
            sizeof(ButtonEvent));

    buttonTaskInit();
    ledTaskInit();
    Serial.begin(115200);

    while (!Serial)
    {
        delay(10);
    }

    LOGI("Controller starting");

    xTaskCreate(
        buttonTask,
        "BTN",
        256,
        NULL,
        3,
        NULL);

    xTaskCreate(
        radioTask,
        "RAD",
        512,
        NULL,
        1,
        NULL);

    xTaskCreate(
        ledTask,
        "LED",
        256,
        NULL,
        2,
        NULL);

    vTaskStartScheduler();
}

void loop()
{
}