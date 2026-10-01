#ifndef MAOXIU_MOTOR_RESOURCES_H
#define MAOXIU_MOTOR_RESOURCES_H
/* Original wheel order; signs retain the original encoder and bridge polarity. */
#define MAOXIU_ENCODER_TIMERS {&htim2,&htim3,&htim4,&htim5}
#define MAOXIU_ENCODER_SIGNS {1,1,-1,-1}
#define MAOXIU_BRIDGE_A_TIMERS {&htim10,&htim9,&htim1,&htim1}
#define MAOXIU_BRIDGE_B_TIMERS {&htim11,&htim9,&htim1,&htim1}
#define MAOXIU_BRIDGE_A_CHANNELS {TIM_CHANNEL_1,TIM_CHANNEL_1,TIM_CHANNEL_1,TIM_CHANNEL_3}
#define MAOXIU_BRIDGE_B_CHANNELS {TIM_CHANNEL_1,TIM_CHANNEL_2,TIM_CHANNEL_2,TIM_CHANNEL_4}
#define MAOXIU_BRIDGE_FORWARD_A {1,1,0,0}
#endif
