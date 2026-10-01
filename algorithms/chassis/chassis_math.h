#ifndef CHASSIS_MATH_H
#define CHASSIS_MATH_H
typedef struct { float vx_mps, vy_mps, wz_radps; } chassis_velocity_t;
typedef struct { float kp,ki,output,previous_error,limit; } chassis_pi_t;
/* Wheels are A/B/C/D as in the STM32 baseline. half_span is half track + half wheelbase.
 * Differential uses left/right, full track in metres. Caller guarantees positive geometry.
 * No SDK, RTOS, allocation or global state. */
void chassis_mecanum_inverse(chassis_velocity_t v,float half_span,float wheels[4]);
chassis_velocity_t chassis_mecanum_forward(const float wheels[4],float half_span);
void chassis_differential_inverse(chassis_velocity_t v,float track,float wheels[2]);
chassis_velocity_t chassis_differential_forward(const float wheels[2],float track);
float chassis_pi_step(chassis_pi_t *pi,float measured,float target);
void chassis_pi_reset(chassis_pi_t *pi);
#endif
