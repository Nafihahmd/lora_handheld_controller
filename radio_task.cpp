#include "radio_task.h"
#include "app.h"

void radioTask(void *pv)
{
    ButtonEvent evt;

    while(1)
    {
        if(xQueueReceive(
            buttonQueue,
            &evt,
            portMAX_DELAY))
        {
            ledStates[evt.button] =
                LED_WAITING;

            if(evt.type == BTN_SHORT)
            {
                // Trigger actuator

                vTaskDelay(
                    pdMS_TO_TICKS(500));

                ledStates[evt.button] =
                    LED_TRIGGERED;
            }
            else
            {
                // Status query

                vTaskDelay(
                    pdMS_TO_TICKS(200));

                ledStates[evt.button] =
                    LED_READY;
            }
        }
    }
}