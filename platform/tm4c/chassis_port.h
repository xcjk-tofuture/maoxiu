#ifndef MAOXIU_CHASSIS_PORT_H
#define MAOXIU_CHASSIS_PORT_H
#include "chassis_service.h"
int chassis_port_init(void);
int chassis_port_submit(chassis_velocity_t velocity);
void chassis_port_snapshot(chassis_snapshot_t *snapshot);
void maoxiu_control_task(void *argument);
#endif
