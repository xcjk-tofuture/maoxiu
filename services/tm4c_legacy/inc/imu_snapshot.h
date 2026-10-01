#ifndef TM4C_IMU_SNAPSHOT_H
#define TM4C_IMU_SNAPSHOT_H
typedef struct {
    float roll, pitch, yaw;
} tm4c_imu_snapshot_t;
void tm4c_imu_snapshot(tm4c_imu_snapshot_t *out);
#endif
