#ifndef CHASSIS_PORT_H
#define CHASSIS_PORT_H
#include "chassis_service.h"
/* Called before scheduler startup; failure keeps outputs disabled. */
int chassis_port_init(void);
/* Task context only. Nonblocking bounded queue, 0=accepted, negative=busy/range/state. */
int chassis_port_submit(chassis_velocity_t target);
/* Copies a complete last-control-tick snapshot under RTOS protection. */
void chassis_port_snapshot(chassis_snapshot_t *snapshot);
#endif
