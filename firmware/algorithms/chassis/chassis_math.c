#include "chassis_math.h"
void chassis_mecanum_inverse(chassis_velocity_t v, float l, float w[4]) {
    w[0] = v.vx_mps + v.vy_mps - v.wz_radps * l;
    w[1] = v.vx_mps - v.vy_mps - v.wz_radps * l;
    w[2] = v.vx_mps - v.vy_mps + v.wz_radps * l;
    w[3] = v.vx_mps + v.vy_mps + v.wz_radps * l;
}
chassis_velocity_t chassis_mecanum_forward(const float w[4], float l) {
    chassis_velocity_t v;
    v.vx_mps = (w[0] + w[2]) * 0.5f;
    v.vy_mps = (w[0] - w[1]) * 0.5f;
    v.wz_radps = (w[3] - w[0]) / (2.0f * l);
    return v;
}
void chassis_differential_inverse(chassis_velocity_t v, float track, float w[2]) {
    w[0] = v.vx_mps - v.wz_radps * track * 0.5f;
    w[1] = v.vx_mps + v.wz_radps * track * 0.5f;
}
chassis_velocity_t chassis_differential_forward(const float w[2], float track) {
    chassis_velocity_t v;
    v.vx_mps = (w[0] + w[1]) * 0.5f;
    v.vy_mps = 0;
    v.wz_radps = (w[1] - w[0]) / track;
    return v;
}
float chassis_pi_step(chassis_pi_t *p, float measured, float target) {
    float error = target - measured;
    p->output += p->kp * (error - p->previous_error) + p->ki * error;
    if (p->output > p->limit)
        p->output = p->limit;
    if (p->output < -p->limit)
        p->output = -p->limit;
    p->previous_error = error;
    return p->output;
}
void chassis_pi_reset(chassis_pi_t *p) {
    p->output = 0;
    p->previous_error = 0;
}
