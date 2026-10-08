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
/* Variable Declarations */
static void (*log_function)(void);
int _is_init = 0;
osTimerId_t osTim_dataLogging;

/* Function Declarations */
static void log_pointer(void *argument); // The param (void *argument) is required by FreeRTOS for timer callback
static void pend_log_data(void);

static void pend_log_data(void ) {

    float voltage = pendulum_read_voltage();
    float relative_time = (TIMER_PERIOD_MS/1000) * logCount;
    printf("%f,%f", relative_time, voltage);
    logCount++;
    if (logCount >= (2 / (TIMER_PERIOD_MS/1000))) {// call after 2 seconds
        logging_stop();
    }
    printf("\n");
}


void logging_init(void) {
    if (_is_init) {
        return;
    }
    _is_init = 1;
    
    // We set the attributes for the timer to their default values
    osTimerAttr_t  dataLoggingTimerAttr = {
        .name = NULL,
        .attr_bits = 0,
        .cb_mem = NULL,
        .cb_size = 0,
    };

    osTim_dataLogging = osTimerNew(log_pointer, osTimerPeriodic, NULL, &dataLoggingTimerAttr);
}

static void log_pointer(void *argument) {
    UNUSED(argument);

    (*log_function)();
}

void logging_start(void) {

    // Set which 
    log_function = &pend_log_data;

    logCount = 0;
    if (osTim_dataLogging != NULL) {
        osTimerStart(osTim_dataLogging, TIMER_PERIOD_MS);
    }
    else {
        //is there a good way to flag this somehow?
        return;
    }
}

void logging_stop(void) {
    osTimerStop(osTim_dataLogging);
}