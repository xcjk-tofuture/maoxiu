#include "chassis_service.h"
#include <string.h>
void chassis_init(chassis_service_t *s, const chassis_config_t *config) {
    unsigned i;
    memset(s, 0, sizeof(*s));
    s->config = *config;
    s->snapshot.wheels = config->wheels;
    for (i = 0; i < 4; i++) {
        s->pi[i].kp = config->kp;
        s->pi[i].ki = config->ki;
        s->pi[i].limit = config->pwm_limit;
    }
}
void chassis_submit(chassis_service_t *s, chassis_velocity_t target, uint32_t now) {
    s->target = target;
    s->last_command_ms = now;
    s->has_command = 1;
}
void chassis_step(chassis_service_t *s, const float measured[4], float voltage, uint32_t now,
                  float pwm[4]) {
    unsigned i;
    float target[4] = {0};
    s->snapshot.voltage = voltage;
    s->snapshot.velocity = s->config.wheels == 4
                               ? chassis_mecanum_forward(measured, s->config.span_m)
                               : chassis_differential_forward(measured, s->config.span_m);
    if (!(voltage > s->config.min_voltage))
        s->snapshot.state = CHASSIS_UNDERVOLTAGE;
    else if (!s->has_command)
        s->snapshot.state = CHASSIS_IDLE;
    else if ((uint32_t)(now - s->last_command_ms) >= s->config.command_timeout_ms)
        s->snapshot.state = CHASSIS_TIMEOUT;
    else
        s->snapshot.state = CHASSIS_ACTIVE;
    if (s->snapshot.state == CHASSIS_ACTIVE) {
        if (s->config.wheels == 4)
            chassis_mecanum_inverse(s->target, s->config.span_m, target);
        else
            chassis_differential_inverse(s->target, s->config.span_m, target);
    }
    for (i = 0; i < 4; i++) {
        if (i < s->config.wheels && s->snapshot.state == CHASSIS_ACTIVE)
            pwm[i] = chassis_pi_step(&s->pi[i], measured[i], target[i]);
        else {
            chassis_pi_reset(&s->pi[i]);
            pwm[i] = 0;
        }
    }
}
