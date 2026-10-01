"""Instance-owned gyro/acceleration fusion, explicit seconds and SI input."""
import math

class Orientation:
    def __init__(self):
        self.q = [1.0, 0.0, 0.0, 0.0]

    def step(self, gyro, acc, dt):
        if not 0 < dt <= 0.5 or not all(math.isfinite(x) for x in (*gyro, *acc)):
            return self.q
        q0, q1, q2, q3 = self.q
        gx, gy, gz = gyro
        ax, ay, az = acc
        norm = math.sqrt(ax * ax + ay * ay + az * az)
        if norm > 1e-12:
            ax, ay, az = ax / norm, ay / norm, az / norm
            vx, vy, vz = q1 * q3 - q0 * q2, q0 * q1 + q2 * q3, q0 * q0 - 0.5 + q3 * q3
            gx += ay * vz - az * vy
            gy += az * vx - ax * vz
            gz += ax * vy - ay * vx
        gx, gy, gz = gx * dt * 0.5, gy * dt * 0.5, gz * dt * 0.5
        q = [q0 - q1 * gx - q2 * gy - q3 * gz,
             q1 + q0 * gx + q2 * gz - q3 * gy,
             q2 + q0 * gy - q1 * gz + q3 * gx,
             q3 + q0 * gz + q1 * gy - q2 * gx]
        norm = math.sqrt(sum(x * x for x in q))
        self.q = [x / norm for x in q]
        return self.q
