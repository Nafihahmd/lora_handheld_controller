#include "radio_task.h"

#include <RadioLib.h>
#include <SPI.h>

#include "app.h"
#include "protocol.h"

SX1262 radio =
    new Module(
        PIN_NSS,
        PIN_DIO1,
        PIN_RST,
        PIN_BUSY);

static uint8_t txCounter = 0;

void radioTask(void *pv)
{
    int state;
    SPI.setMOSI(LORA_MOSI);
    SPI.setMISO(LORA_MISO);
    SPI.setSCLK(LORA_SCK);
    SPI.begin();

    LOGI("Radio init");

    ConfigLoRa_t config;
    config.frequency = 865.0;
    // state = radio.begin(config);
    
     state = radio.begin(865.0, 125.0, 9, 7, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 10, 8, 0, false);

    if (state != RADIOLIB_ERR_NONE)
    {
        LOGE("Radio init failed (%d)", state);
    }
    else
    {
        LOGI("Radio init OK");
    }

    ButtonEvent evt;

    uint8_t txBuf[16];
    size_t txLen;

    while (1)
    {
        if (xQueueReceive(
                buttonQueue,
                &evt,
                portMAX_DELAY))
        {
            txCounter++;

            ledStates[evt.button] =
                LED_WAITING;

            if (evt.type == BTN_SHORT)
            {
                buildTriggerPacket(
                    evt.button,
                    txCounter,
                    txBuf,
                    &txLen);

                LOGI(
                    "TRIGGER ch=%u cnt=%u",
                    evt.button,
                    txCounter);
            }
            else
            {
                buildStatusRequestPacket(
                    evt.button,
                    txCounter,
                    txBuf,
                    &txLen);

                LOGI(
                    "STATUS ch=%u cnt=%u",
                    evt.button,
                    txCounter);
            }

            state =
                radio.transmit(
                    txBuf,
                    txLen);

            if (state != RADIOLIB_ERR_NONE)
            {
                LOGE(
                    "TX failed %d",
                    state);

                ledStates[evt.button] =
                    LED_ERROR;
            }
        }
    }
}