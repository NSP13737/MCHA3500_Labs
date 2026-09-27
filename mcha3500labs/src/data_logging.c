#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h> // For PRIxx and SCNxx macros
#include "stm32f4xx_hal.h" // to import UNUSED() macro
#include "pendulum.h"
#include "cmsis_os2.h"
#include "data_logging.h"

uint16_t logCount;

#define TIMER_PERIOD_MS (1000.0f/200.0f) // Since the osTim runs at 1kHz, we do this to get 200Hz

static void log_pendulum(void *argument);

static void log_pendulum(void *argument) {
    UNUSED(argument);

    float voltage = pendulum_read_voltage();
    float relative_time = (TIMER_PERIOD_MS/1000) * logCount;
    printf("%f,%f", relative_time, voltage);
    logCount++;
    if (logCount >= (2 / (TIMER_PERIOD_MS/1000))) {// call after 2 seconds
        pend_logging_stop();
    }
    printf("\n");
}

int _is_init = 0;
osTimerId_t osTim_dataLogging;
void logging_init(void) {
    if (_is_init) {
        return;
    }
    _is_init = 1;

    osTim_dataLogging = osTimerNew(log_pendulum, osTimerPeriodic, NULL, NULL);
}

void pend_logging_start(void) {
    logCount = 0;
    if (osTim_dataLogging != NULL) {
        osTimerStart(osTim_dataLogging, TIMER_PERIOD_MS);
    }
    else {
        //is there a good way to flag this somehow?
        return;
    }
}

void pend_logging_stop(void) {
    osTimerStop(osTim_dataLogging);
}