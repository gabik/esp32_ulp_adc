/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

#include "hal/adc_types.h"

#define EXAMPLE_ADC_CHANNEL     ADC_CHANNEL_0
#define EXAMPLE_ADC_UNIT        ADC_UNIT_2
#define EXAMPLE_ADC_ATTEN       ADC_ATTEN_DB_12
#define EXAMPLE_ADC_WIDTH       ADC_BITWIDTH_DEFAULT

#define GPIO_PIN 1
/* Set high threshold, approx. 1.75V*/
#define EXAMPLE_ADC_TRESHOLD    0
