#include "motor.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include <stdio.h>

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


void motor_encoder_init(void) {
    __GPIOC_CLK_ENABLE();

    //init both 0 and 1 (PC0 PC1)
    GPIO_InitTypeDef gpio_config;
    gpio_config.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    gpio_config.Mode = GPIO_MODE_IT_RISING_FALLING;
    gpio_config.Pull = GPIO_NOPULL;
    gpio_config.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOC,&gpio_config);

    HAL_NVIC_SetPriority(EXTI0_IRQn, 0x0f, 0x0f); // set to lowest priority
    HAL_NVIC_SetPriority(EXTI1_IRQn, 0x0f, 0x0f);

    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
    //HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

static volatile int32_t encoder_count = 0;

void EXTI0_IRQHandler(void) {
    printf("EXTI0 Triggered");
    GPIO_PinState gpio_0_pin_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0);
    if (gpio_0_pin_state == HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1)) {
        if (gpio_0_pin_state == GPIO_PIN_SET) {
            encoder_count++;
        }
        else {
            encoder_count--;
        }
    }

    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

int32_t motor_encoder_getValue(void) {
    return encoder_count;
}