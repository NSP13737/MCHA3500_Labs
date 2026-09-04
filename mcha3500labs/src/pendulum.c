#include "pendulum.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"



static uint8_t _is_init = 0;

static ADC_HandleTypeDef adc_config;

void pendulum_init(void) {
    if (_is_init)
    {
        return;
    }
    _is_init = 1;

    __HAL_RCC_ADC1_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();

    // Initialize PB0
    GPIO_InitTypeDef gpio_config;
    gpio_config.Pin = GPIO_PIN_0;
    gpio_config.Mode = GPIO_MODE_ANALOG;
    gpio_config.Pull = GPIO_NOPULL;
    gpio_config.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB,&gpio_config);

    //Initialize ADC1
    adc_config.Instance = ADC1;
    adc_config.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    adc_config.Init.Resolution = ADC_RESOLUTION_12B;
    adc_config.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc_config.Init.ContinuousConvMode = ENABLE;
    adc_config.Init.NbrOfConversion = 1;
    HAL_ADC_Init(&adc_config);

    ADC_ChannelConfTypeDef adc_channelconf;
    adc_channelconf.Channel = ADC_CHANNEL_8;
    adc_channelconf.Rank = 1;
    adc_channelconf.SamplingTime = ADC_SAMPLETIME_480CYCLES;
    adc_channelconf.Offset = 0;
    HAL_ADC_ConfigChannel(&adc_config, &adc_channelconf);
    
}

float pendulum_read_voltage(void) {
    HAL_ADC_Start(&adc_config);
    HAL_ADC_PollForConversion(&adc_config, 0xFF);
    float adc_val = HAL_ADC_GetValue(&adc_config);
    HAL_ADC_Stop(&adc_config);
    // We set adc to have 12bit range (4095 vals), and we have to scale it by VDD, which
    // for our board is 3.3V
    float voltage = (adc_val/4095) * 3.3;
    return voltage;
}