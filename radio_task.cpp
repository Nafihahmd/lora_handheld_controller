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

struct PendingRequest
{
    bool active;
    uint8_t counter;
    uint8_t channel;
    uint32_t startMs;
};

static PendingRequest pending = {0};

volatile bool rxFlag = false;

void setRxFlag()
{
    rxFlag = true;
}

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

    radio.setPacketReceivedAction(setRxFlag);
    radio.startReceive();

    ButtonEvent evt;

    uint8_t txBuf[16];
    size_t txLen;
    while (1)
    {
        if (xQueueReceive(
                buttonQueue,
                &evt,
                pdMS_TO_TICKS(20)))
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

            state = radio.transmit(
                txBuf,
                txLen);

            if (state != RADIOLIB_ERR_NONE)
            {
                LOGE(
                    "TX failed %d",
                    state);

                ledStates[evt.button] =
                    LED_ERROR;

                continue;
            }

            pending.active = true;
            pending.counter = txCounter;
            pending.channel = evt.button;
            pending.startMs = millis();

            radio.startReceive();
        }

        if (rxFlag)
        {
            rxFlag = false;

            uint8_t rxBuf[32];

            int packetLength =
                radio.getPacketLength();

            state =
                radio.readData(
                    rxBuf,
                    packetLength);

            if (state == RADIOLIB_ERR_NONE)
            {
                AckPayload ack;

                if (decodeAckPacket(
                        rxBuf,
                        packetLength,
                        &ack))
                {
                    LOGI(
                        "ACK ch=%u cnt=%u res=%u",
                        ack.channel,
                        ack.counter,
                        ack.result);

                    if (pending.active &&
                        ack.counter ==
                            pending.counter)
                    {
                        switch (ack.result)
                        {
                            case RES_ACCEPTED:
                            case RES_ALREADY_FIRED:

                                ledStates[
                                    ack.channel] =
                                    LED_TRIGGERED;
                                break;

                            default:

                                ledStates[
                                    ack.channel] =
                                    LED_ERROR;
                                break;
                        }

                        pending.active = false;
                    }
                }

                StatusPayload status;

                if (decodeStatusPacket(
                        rxBuf,
                        packetLength,
                        &status))
                {
                    LOGI(
                        "STATUS ch=%u cnt=%u state=%u batt=%u",
                        status.channel,
                        status.counter,
                        status.state,
                        status.battery);

                    if (pending.active &&
                        status.counter ==
                            pending.counter)
                    {
                        switch (status.state)
                        {
                            case STATE_READY:

                                ledStates[
                                    status.channel] =
                                    LED_READY;
                                break;

                            case STATE_FIRED:

                                ledStates[
                                    status.channel] =
                                    LED_TRIGGERED;
                                break;

                            default:

                                ledStates[
                                    status.channel] =
                                    LED_ERROR;
                                break;
                        }

                        pending.active = false;

                        if (status.battery < 20)
                        {
                            ledStates[
                                status.channel] =
                                LED_LOW_BATTERY;

                            vTaskDelay(
                                pdMS_TO_TICKS(4000));

                            switch (
                                status.state)
                            {
                                case STATE_READY:

                                    ledStates[
                                        status.channel] =
                                        LED_READY;
                                    break;

                                case STATE_FIRED:

                                    ledStates[
                                        status.channel] =
                                        LED_TRIGGERED;
                                    break;
                            }
                        }
                    }
                }
            }

            radio.startReceive();
        }

        if (pending.active)
        {
            if ((millis() -
                pending.startMs) >
                3000)
            {
                LOGE(
                    "Timeout ch=%u cnt=%u",
                    pending.channel,
                    pending.counter);

                ledStates[
                    pending.channel] =
                    LED_TIMEOUT;

                pending.active = false;
            }
        }
    }
}