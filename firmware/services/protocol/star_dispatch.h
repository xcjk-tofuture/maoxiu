#ifndef STAR_DISPATCH_H
#define STAR_DISPATCH_H
#include "star_protocol.h"
typedef uint8_t (*star_business_fn)(void *,const star_frame_t *,star_frame_t *);
typedef struct {
    uint32_t device_id, capabilities;
    uint16_t telemetry_period_ms, minimum_period_ms;
    star_business_fn business;
    void *context;
} star_dispatch_t;
/* Called only by the communication task. Response starts with status byte.
 * Parameter 1 is telemetry interval in milliseconds, volatile until reset.
 * Business callback returns a STAR_* status; data starts at payload[1]. */
void star_dispatch(star_dispatch_t *service,const star_frame_t *request,star_frame_t *response);
#endif
