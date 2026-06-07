#include "protocol.h"

static uint16_t crc16_ccitt(
    const uint8_t *data,
    size_t len)
{
    uint16_t crc = 0xFFFF;

    while (len--)
    {
        crc ^= ((uint16_t)*data++) << 8;

        for (uint8_t i = 0; i < 8; i++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }

    return crc;
}

bool buildTriggerPacket(
    uint8_t channel,
    uint8_t counter,
    uint8_t *buffer,
    size_t *length)
{
    CmdFrame frame;

    frame.header.netId = PROTO_NET_ID;
    frame.header.ttl   = PROTO_TTL_INIT;

    frame.payload.type    = FRAME_CMD;
    frame.payload.counter = counter;
    frame.payload.channel = channel;
    frame.payload.command = CMD_TRIGGER;

    frame.payload.crc16 =
        crc16_ccitt(
            (uint8_t *)&frame.payload,
            sizeof(CmdPayload) - sizeof(uint16_t));

    memcpy(buffer,
           &frame,
           sizeof(frame));

    *length = sizeof(frame);

    return true;
}

bool buildStatusRequestPacket(
    uint8_t channel,
    uint8_t counter,
    uint8_t *buffer,
    size_t *length)
{
    CmdFrame frame;

    frame.header.netId = PROTO_NET_ID;
    frame.header.ttl   = PROTO_TTL_INIT;

    frame.payload.type    = FRAME_CMD;
    frame.payload.counter = counter;
    frame.payload.channel = channel;
    frame.payload.command = CMD_STATUS;

    frame.payload.crc16 =
        crc16_ccitt(
            (uint8_t *)&frame.payload,
            sizeof(CmdPayload) - sizeof(uint16_t));

    memcpy(buffer,
           &frame,
           sizeof(frame));

    *length = sizeof(frame);

    return true;
}

bool decodeAckPacket(
    const uint8_t *buffer,
    size_t length,
    AckPayload *ack)
{
    if(length != sizeof(AckFrame))
        return false;

    const AckFrame *frame =
        (const AckFrame *)buffer;

    if(frame->header.netId !=
       PROTO_NET_ID)
    {
        return false;
    }

    if(frame->payload.type !=
       FRAME_ACK)
    {
        return false;
    }

    *ack = frame->payload;

    return true;
}

bool decodeStatusPacket(
    const uint8_t *buffer,
    size_t length,
    StatusPayload *status)
{
    if(length != sizeof(StatusFrame))
        return false;

    const StatusFrame *frame =
        (const StatusFrame *)buffer;

    if(frame->header.netId !=
       PROTO_NET_ID)
    {
        return false;
    }

    if(frame->payload.type !=
       FRAME_STATUS)
    {
        return false;
    }

    *status = frame->payload;

    return true;
}

