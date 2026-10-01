#ifndef CHASSIS_PARAMETERS_H
#define CHASSIS_PARAMETERS_H
#include "chassis_service.h"
#define CP_BYTES 36u
typedef struct {
    chassis_config_t control;
    float speed_scale;
    uint16_t telemetry_period_ms;
} chassis_parameters_t;
void cp_defaults(chassis_parameters_t *p);
int cp_valid(const chassis_parameters_t *p);
int cp_set(chassis_parameters_t *p, uint16_t id, float value);
int cp_get(const chassis_parameters_t *p, uint16_t id, float *value);
void cp_encode(const chassis_parameters_t *p, uint8_t bytes[CP_BYTES]);
int cp_decode(chassis_parameters_t *p, const uint8_t bytes[CP_BYTES]);
#endif
