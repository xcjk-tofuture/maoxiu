#ifndef CHASSIS_SERVICE_H
#define CHASSIS_SERVICE_H
#include <stdint.h>
#include "chassis_math.h"
enum { CHASSIS_IDLE=0, CHASSIS_ACTIVE=1, CHASSIS_UNDERVOLTAGE=2, CHASSIS_TIMEOUT=3, CHASSIS_UNCALIBRATED=4 };
typedef struct {
    uint8_t wheels;
    float span_m,kp,ki,pwm_limit,min_voltage;
    uint32_t command_timeout_ms;
} chassis_config_t;
typedef struct {
    chassis_velocity_t velocity;
    uint8_t state,wheels;
    float voltage;
} chassis_snapshot_t;
typedef struct {
    chassis_config_t config;
    chassis_pi_t pi[4];
    chassis_velocity_t target;
    chassis_snapshot_t snapshot;
    uint32_t last_command_ms;
    uint8_t has_command;
} chassis_service_t;
/* Control task is the sole writer. Submit and step are nonblocking; now_ms
 * is monotonic milliseconds. Platform adapter validates finite SI values. */
void chassis_init(chassis_service_t *service,const chassis_config_t *config);
void chassis_submit(chassis_service_t *service,chassis_velocity_t target,uint32_t now_ms);
void chassis_step(chassis_service_t *service,const float measured[4],float voltage,
                  uint32_t now_ms,float pwm[4]);
#endif
