#include "chassis_parameters.h"
#include "maoxiu_board.h"
#include "star_protocol.h"
#include <math.h>
void cp_defaults(chassis_parameters_t *p) {
    p->control = (chassis_config_t){4,
                                    MAOXIU_HALF_SPAN_M,
                                    MAOXIU_KP,
                                    MAOXIU_KI,
                                    MAOXIU_PWM_LIMIT,
                                    MAOXIU_MIN_VOLTAGE,
                                    MAOXIU_COMMAND_TIMEOUT_MS};
    p->speed_scale = 1;
    p->telemetry_period_ms = 100;
}
int cp_valid(const chassis_parameters_t *p) {
    const chassis_config_t *c = &p->control;
    return c->wheels == 4 && isfinite(c->span_m) && c->span_m >= 0.05f && c->span_m <= 1 &&
           isfinite(c->kp) && c->kp >= 0 && c->kp <= 10000 && isfinite(c->ki) && c->ki >= 0 &&
           c->ki <= 2000 && isfinite(c->pwm_limit) && c->pwm_limit >= 1 &&
           c->pwm_limit <= MAOXIU_PWM_LIMIT && isfinite(c->min_voltage) &&
           c->min_voltage >= MAOXIU_MIN_VOLTAGE && c->min_voltage <= 24 &&
           c->command_timeout_ms >= 50 && c->command_timeout_ms <= MAOXIU_COMMAND_TIMEOUT_MS &&
           isfinite(p->speed_scale) && p->speed_scale >= 0.25f && p->speed_scale <= 4 &&
           p->telemetry_period_ms >= 50 && p->telemetry_period_ms <= 1000;
}
int cp_set(chassis_parameters_t *p, uint16_t id, float v) {
    chassis_parameters_t next = *p;
    if (!isfinite(v))
        return -1;
    switch (id) {
    case 0x100:
        next.control.kp = v;
        break;
    case 0x101:
        next.control.ki = v;
        break;
    case 0x102:
        next.control.span_m = v;
        break;
    case 0x103:
        next.control.pwm_limit = v;
        break;
    case 0x104:
        next.control.min_voltage = v;
        break;
    case 0x105:
        if (v < 50 || v > MAOXIU_COMMAND_TIMEOUT_MS || floorf(v) != v)
            return -1;
        next.control.command_timeout_ms = (uint32_t)v;
        break;
    case 0x106:
        next.speed_scale = v;
        break;
    default:
        return -2;
    }
    if (!cp_valid(&next))
        return -1;
    *p = next;
    return 0;
}
int cp_get(const chassis_parameters_t *p, uint16_t id, float *v) {
    switch (id) {
    case 0x100:
        *v = p->control.kp;
        break;
    case 0x101:
        *v = p->control.ki;
        break;
    case 0x102:
        *v = p->control.span_m;
        break;
    case 0x103:
        *v = p->control.pwm_limit;
        break;
    case 0x104:
        *v = p->control.min_voltage;
        break;
    case 0x105:
        *v = (float)p->control.command_timeout_ms;
        break;
    case 0x106:
        *v = p->speed_scale;
        break;
    default:
        return -2;
    }
    return 0;
}
void cp_encode(const chassis_parameters_t *p, uint8_t b[CP_BYTES]) {
    star_write_u32(b, 4);
    star_write_f32(b + 4, p->control.span_m);
    star_write_f32(b + 8, p->control.kp);
    star_write_f32(b + 12, p->control.ki);
    star_write_f32(b + 16, p->control.pwm_limit);
    star_write_f32(b + 20, p->control.min_voltage);
    star_write_u32(b + 24, p->control.command_timeout_ms);
    star_write_f32(b + 28, p->speed_scale);
    star_write_u32(b + 32, p->telemetry_period_ms);
}
int cp_decode(chassis_parameters_t *p, const uint8_t b[CP_BYTES]) {
    chassis_parameters_t next;
    if (star_read_u32(b) != 4 || star_read_u32(b + 32) > 1000)
        return -1;
    next.control.wheels = 4;
    next.control.span_m = star_read_f32(b + 4);
    next.control.kp = star_read_f32(b + 8);
    next.control.ki = star_read_f32(b + 12);
    next.control.pwm_limit = star_read_f32(b + 16);
    next.control.min_voltage = star_read_f32(b + 20);
    next.control.command_timeout_ms = star_read_u32(b + 24);
    next.speed_scale = star_read_f32(b + 28);
    next.telemetry_period_ms = (uint16_t)star_read_u32(b + 32);
    if (!cp_valid(&next))
        return -1;
    *p = next;
    return 0;
}
