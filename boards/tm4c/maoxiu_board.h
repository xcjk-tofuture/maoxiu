#ifndef MAOXIU_TM4C_BOARD_H
#define MAOXIU_TM4C_BOARD_H
/* Old board has two QEI channels. Calibration values are intentionally unset.
 * No motion capability is advertised until hardware measurements validate them. */
#define MAOXIU_TM4C_CONTROL_MS 10u
#define MAOXIU_TM4C_WHEELS 2u
#define MAOXIU_TM4C_CALIBRATED 0
#define MAOXIU_TM4C_TRACK_M 0.0f
#define MAOXIU_TM4C_METRES_PER_ENCODER_COUNT 0.0f
#define MAOXIU_TM4C_MIN_VOLTAGE 6.0f
#define MAOXIU_TM4C_KP 0.0f
#define MAOXIU_TM4C_KI 0.0f
#define MAOXIU_TM4C_ADC_VOLTS_PER_COUNT (33.0f/4096.0f)
#endif
