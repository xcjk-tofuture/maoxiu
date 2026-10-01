#include "motor_hal.h"
#include "tim.h"
#include "maoxiu_board.h"
#include "motor_resources.h"
static TIM_HandleTypeDef *const encoders[4] = MAOXIU_ENCODER_TIMERS;
static const int signs[4] = MAOXIU_ENCODER_SIGNS;
static TIM_HandleTypeDef *const bridge_a[4] = MAOXIU_BRIDGE_A_TIMERS;
static TIM_HandleTypeDef *const bridge_b[4] = MAOXIU_BRIDGE_B_TIMERS;
static const uint32_t channel_a[4] = MAOXIU_BRIDGE_A_CHANNELS;
static const uint32_t channel_b[4] = MAOXIU_BRIDGE_B_CHANNELS;
static const uint8_t forward_a[4] = MAOXIU_BRIDGE_FORWARD_A;
void maoxiu_motor_hal_encoder_init(void) {
    for (unsigned i = 0; i < 4; i++) {
        HAL_TIM_Encoder_Start(encoders[i], TIM_CHANNEL_1);
        HAL_TIM_Encoder_Start(encoders[i], TIM_CHANNEL_2);
        HAL_TIM_Base_Start_IT(encoders[i]);
    }
}
void maoxiu_motor_hal_read(maoxiu_motor_hal_sample_t *motor) {
    if (!motor || motor->Num >= 4)
        return;
    TIM_HandleTypeDef *timer = encoders[motor->Num];
    int16_t count = (int16_t)__HAL_TIM_GET_COUNTER(timer);
    __HAL_TIM_SET_COUNTER(timer, 0);
    motor->Direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(timer);
    motor->Trig = -count;
    motor->Speed = (float)count * signs[motor->Num] * MAOXIU_ENCODER_HZ * MAOXIU_WHEEL_PERIMETER_M /
                   MAOXIU_ENCODER_COUNTS;
}
void maoxiu_motor_hal_init(void) {
    for (unsigned i = 0; i < 4; i++) {
        HAL_TIM_PWM_Start(bridge_a[i], channel_a[i]);
        HAL_TIM_PWM_Start(bridge_b[i], channel_b[i]);
        __HAL_TIM_SET_COMPARE(bridge_a[i], channel_a[i], 0);
        __HAL_TIM_SET_COMPARE(bridge_b[i], channel_b[i], 0);
    }
}
void maoxiu_motor_hal_write(maoxiu_motor_hal_sample_t *motor, int16_t speed) {
    if (!motor || motor->Num >= 4)
        return;
    unsigned i = motor->Num;
    if (speed > MAOXIU_PWM_PERIOD)
        speed = MAOXIU_PWM_PERIOD;
    if (speed < -MAOXIU_PWM_PERIOD)
        speed = -MAOXIU_PWM_PERIOD;
    motor->SetSpeedRange = speed;
    motor->SetDirection = speed < 0;
    uint32_t low = MAOXIU_PWM_PERIOD - (speed < 0 ? -speed : speed);
    uint8_t lower_a = (speed >= 0) == forward_a[i];
    __HAL_TIM_SET_COMPARE(bridge_a[i], channel_a[i], lower_a ? low : MAOXIU_PWM_PERIOD);
    __HAL_TIM_SET_COMPARE(bridge_b[i], channel_b[i], lower_a ? MAOXIU_PWM_PERIOD : low);
}
