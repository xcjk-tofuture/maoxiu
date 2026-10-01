#include "chassis_parameters.h"
#include "star_dispatch.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
static unsigned calls;
static uint8_t callback(void *ctx, const star_frame_t *q, star_frame_t *r) {
    (void)ctx;
    (void)q;
    (void)r;
    calls++;
    return STAR_RANGE;
}
int main(void) {
    chassis_parameters_t p, q, before;
    uint8_t bytes[CP_BYTES];
    cp_defaults(&p);
    assert(cp_valid(&p));
    cp_encode(&p, bytes);
    assert(cp_decode(&q, bytes) == 0);
    cp_encode(&q, bytes);
    float f;
    assert(cp_get(&q, 0x100, &f) == 0 && f == p.control.kp);
    const float bad[] = {NAN, INFINITY, -1, 1e9f};
    before = p;
    for (unsigned i = 0; i < 4; i++) {
        assert(cp_set(&p, 0x100, bad[i]) == -1);
        assert(p.control.kp == before.control.kp);
    }
    assert(cp_set(&p, 0x105, 50.5f) == -1);
    assert(cp_set(&p, 0x105, 501) == -1);
    assert(cp_set(&p, 0x104, 5.9f) == -1);
    assert(cp_set(&p, 0x106, 0.25f) == 0);
    assert(cp_set(&p, 0x999, 1) == -2);
    bytes[0] = 3;
    assert(cp_decode(&p, bytes) == -1);
    star_dispatch_t d = {.business = callback, .telemetry_period_ms = 100, .minimum_period_ms = 50};
    star_frame_t request = {.command = STAR_CMD_PARAM_WRITE, .length = 6}, response;
    star_write_u16(request.payload, 0x100);
    star_dispatch(&d, &request, &response);
    assert(calls == 1 && response.payload[0] == STAR_RANGE);
    request.length = 1;
    star_dispatch(&d, &request, &response);
    assert(response.payload[0] == STAR_BAD_LENGTH && calls == 1);
    request.length = 6;
    star_write_u16(request.payload, 1);
    star_dispatch(&d, &request, &response);
    assert(response.payload[0] == STAR_BAD_LENGTH);
    puts("PASS chassis parameters: roundtrip, finite/range guards, safety ceilings and delegated "
         "extension lengths");
    return 0;
}
