#include "button_task.h"
#include "app.h"

#include <AceButton.h>

using namespace ace_button;

static const uint8_t buttonPins[NUM_BUTTONS] = {
    PB2,
    PA3,
    PC6,
    PA1,
    PA15,
    PA2
};

static ButtonConfig buttonConfig;

static AceButton buttons[NUM_BUTTONS] = {
    AceButton(&buttonConfig, PB2, HIGH, 0),
    AceButton(&buttonConfig, PA3, HIGH, 1),
    AceButton(&buttonConfig, PC6, HIGH, 2),
    AceButton(&buttonConfig, PA1, HIGH, 3),
    AceButton(&buttonConfig, PA15, HIGH, 4),
    AceButton(&buttonConfig, PA2, HIGH, 5)
};

static void handleEvent(
    AceButton* button,
    uint8_t eventType,
    uint8_t)
{
    ButtonEvent evt;

    evt.button = button->getId();

    switch(eventType)
    {
        case AceButton::kEventClicked:

            evt.type = BTN_SHORT;

            tone(BUZZER_PIN, 4000, 40);

            xQueueSend(buttonQueue, &evt, 0);
            break;

        case AceButton::kEventLongPressed:

            evt.type = BTN_LONG;

            tone(BUZZER_PIN, 4000, 100);

            xQueueSend(buttonQueue, &evt, 0);
            break;
    }
}

void buttonTaskInit(void)
{
    for(int i=0;i<NUM_BUTTONS;i++)
    {
        pinMode(buttonPins[i], INPUT_PULLUP);
    }

    pinMode(BUZZER_PIN, OUTPUT);

    buttonConfig.setEventHandler(handleEvent);
    buttonConfig.setFeature(ButtonConfig::kFeatureClick);
    buttonConfig.setFeature(ButtonConfig::kFeatureLongPress);

    buttonConfig.setLongPressDelay(1000);
}

void buttonTask(void *pv)
{
    TickType_t lastWake =
        xTaskGetTickCount();

    while(1)
    {
        for(int i=0;i<NUM_BUTTONS;i++)
        {
            buttons[i].check();
        }

        vTaskDelayUntil(
            &lastWake,
            pdMS_TO_TICKS(1));
    }
}