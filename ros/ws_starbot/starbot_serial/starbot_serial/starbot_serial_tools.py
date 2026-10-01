import math


def check_sum_BBC(Count_Number,Receive_Data=None,):

    check_sum = 0

    for k in range(Count_Number):
        check_sum ^= Receive_Data[k]  # 按位异或

    return check_sum  # 返回按位异或结果

def imu_trans(data_high, data_low):

    """

    将两个8位无符号整数合并为一个16位整数

    """
    transition_16 = (data_high << 8) | data_low
    if transition_16 >= 0x8000:  # 判断是否为负数（16位有符号整数中，最高位为1表示负数）
    # 负数转换为m/s
        transition_16 = (transition_16 - 0x10000)

    return transition_16


def odom_trans(data_high, data_low):

    """

    将两个8位无符号整数合并为一个16位整数，并将其表示的速度从mm/s转换为m/s

    """

    transition_16 = (data_high << 8) | data_low  # 使用imu_trans处理正负数
    # 速度单位从mm/s转换为m/s

    # 速度单位从mm/s转换为m/s，同时正确处理正负

    if transition_16 >= 0x8000:  # 判断是否为负数（16位有符号整数中，最高位为1表示负数）
        # 负数转换为m/s
        data_return = (transition_16 - 0x10000) / 1000
    else:
        # 正数转换为m/s
        data_return = transition_16 / 1000
    return data_return





def inv_sqrt(x):

    """Approximate inverse square root."""

    return 1.0 / math.sqrt(x)



two_kp = 1.0  # 2 * proportional gain (Kp)
two_ki = 0.0  # 2 * integral gain (Ki)
q0, q1, q2, q3 = 1.0, 0.0, 0.0, 0.0  # quaternion of sensor frame relative to auxiliary frame
integral_fb_x = 0.0
integral_fb_y = 0.0
integral_fb_z = 0.0  # integral error terms scaled by Ki

def quaternion_solution(gx, gy, gz, ax, ay, az, sampling_freq=20.0):
    global q0, q1, q2, q3, integral_fb_x, integral_fb_y, integral_fb_z,two_kp, two_ki  # 声明所有外部引用的全局变量
    """Quaternion-based solution for sensor fusion."""
    recip_norm = inv_sqrt(ax * ax + ay * ay + az * az)
    ax *= recip_norm
    ay *= recip_norm
    az *= recip_norm

    half_vx = q1 * q3 - q0 * q2
    half_vy = q0 * q1 + q2 * q3
    half_vz = q0 * q0 - 0.5 + q3 * q3

    half_ex = ay * half_vz - az * half_vy
    half_ey = az * half_vx - ax * half_vz
    half_ez = ax * half_vy - ay * half_vx 

    if two_ki > 0.0:

        integral_fb_x += two_ki * half_ex * (1.0 / sampling_freq)
        integral_fb_y += two_ki * half_ey * (1.0 / sampling_freq)
        integral_fb_z += two_ki * half_ez * (1.0 / sampling_freq)

        gx += integral_fb_x
        gy += integral_fb_y
        gz += integral_fb_z

    else:

        integral_fb_x = 0.0
        integral_fb_y = 0.0
        integral_fb_z = 0.0

    

    gx += two_kp * half_ex
    gy += two_kp * half_ey
    gz += two_kp * half_ez

    gx *= 0.5 / sampling_freq
    gy *= 0.5 / sampling_freq
    gz *= 0.5 / sampling_freq

    qa, qb, qc = q0, q1, q2 

    q0 += (-qb * gx - qc * gy - q3 * gz)
    q1 += (qa * gx + qc * gz - q3 * gy)
    q2 += (qa * gy - qb * gz + q3 * gx)
    q3 += (qa * gz + qb * gy - qc * gx)

    recip_norm = inv_sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3)

    q0 *= recip_norm

    q1 *= recip_norm

    q2 *= recip_norm

    q3 *= recip_norm

    return_list = [q0, q1, q2, q3]
    return  return_list

   


