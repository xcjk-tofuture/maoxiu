#ifndef CHASSIS_PORT_H
#define CHASSIS_PORT_H
#include "chassis_parameters.h"
int chassis_port_init(void);
int chassis_port_submit(chassis_velocity_t target);
void chassis_port_snapshot(chassis_snapshot_t *snapshot);
/* Task-only, nonblocking. PC task is the parameter writer. Updates are queued by
 * value and applied by the control owner at a stopped control tick. Values read
 * are the applied snapshot, with monotonic revision. -2 range, -3 state, -1 busy. */
int chassis_port_parameter_set(uint16_t id, float value);
void chassis_port_parameters(chassis_parameters_t *p, uint32_t *revision);
/* Begin prevents new commands/config writes atomically. Ready means PWM is off.
 * PC task releases after durable save; release clears old queued commands. */
int chassis_port_maintenance_begin(void);
int chassis_port_maintenance_ready(void);
void chassis_port_maintenance_end(void);
uint32_t chassis_port_overruns(void);
#endif
