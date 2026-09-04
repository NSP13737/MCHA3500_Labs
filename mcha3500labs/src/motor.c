#include "motor.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"

static uint8_t _is_init = 0;

static TIM_HandleTypeDef htim3;

void motor_PWM_init(void) {
    if (_is_init) {
        return;
    } 
    _is_init = 1;

    __TIM3_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();

    // configure pins in AF mode
    GPIO_InitTypeDef gpio_config;
    gpio_config.Pin = GPIO_PIN_6;
    gpio_config.Mode = GPIO_MODE_AF_PP;
    gpio_config.Pull = GPIO_NOPULL;
    gpio_config.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_config.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA,&gpio_config);

    // Set up timer in PWM mode
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = TIM_CLOCKPRESCALER_DIV1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    // \frac{\left(180\cdot10^{6}\right)}{10\cdot10^{3}}-1
    htim3.Init.Period = 9999;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim3);

    // config timer more
    TIM_OC_InitTypeDef tim3_PWM;
    tim3_PWM.OCMode = TIM_OCMODE_PWM1;
    tim3_PWM.Pulse = 0;
    tim3_PWM.OCPolarity = TIM_OCPOLARITY_HIGH;
    tim3_PWM.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim3, &tim3_PWM, TIM_CHANNEL_1);

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 2500);

    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);


}