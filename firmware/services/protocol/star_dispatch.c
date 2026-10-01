#include "star_dispatch.h"
#include <string.h>
void star_dispatch(star_dispatch_t *s, const star_frame_t *q, star_frame_t *r) {
    uint8_t result = STAR_OK;
    uint16_t value;
    memset(r, 0, sizeof(*r));
    r->flags = STAR_RESPONSE;
    r->sequence = q->sequence;
    r->command = q->command;
    r->length = 1;
    if (q->flags != STAR_REQUEST)
        result = STAR_STATE;
    else
        switch (q->command) {
        case STAR_CMD_VERSION:
            if (q->length)
                result = STAR_BAD_LENGTH;
            else {
                r->payload[1] = STAR_VERSION;
                r->length = 2;
            }
            break;
        case STAR_CMD_IDENTIFY:
            if (q->length)
                result = STAR_BAD_LENGTH;
            else {
                star_write_u32(r->payload + 1, s->device_id);
                r->length = 5;
            }
            break;
        case STAR_CMD_CAPABILITIES:
            if (q->length)
                result = STAR_BAD_LENGTH;
            else {
                star_write_u32(r->payload + 1, s->capabilities);
                r->length = 5;
            }
            break;
        case STAR_CMD_PARAM_READ:
            if (q->length != 2)
                result = STAR_BAD_LENGTH;
            else if (star_read_u16(q->payload) != 1)
                result = STAR_UNSUPPORTED;
            else {
                star_write_u16(r->payload + 1, s->telemetry_period_ms);
                r->length = 3;
            }
            break;
        case STAR_CMD_PARAM_WRITE:
            if (q->length != 4)
                result = STAR_BAD_LENGTH;
            else if (star_read_u16(q->payload) != 1)
                result = STAR_UNSUPPORTED;
            else {
                value = star_read_u16(q->payload + 2);
                if (value < s->minimum_period_ms || value > 1000)
                    result = STAR_RANGE;
                else
                    s->telemetry_period_ms = value;
            }
            break;
        default:
            result = s->business ? s->business(s->context, q, r) : STAR_UNSUPPORTED;
            break;
        }
    if (result != STAR_OK || r->length > STAR_PAYLOAD_MAX) {
        if (r->length > STAR_PAYLOAD_MAX)
            result = STAR_INTERNAL;
        r->length = 1;
    }
    r->payload[0] = result;
}
