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
        2,
        NULL);

    xTaskCreate(
        ledTask,
        "LED",
        256,
        NULL,
        1,
        NULL);

    vTaskStartScheduler();
}

void loop()
{
}