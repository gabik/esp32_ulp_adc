/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* ULP-RISC-V example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.

   This code runs on ULP-RISC-V  coprocessor
*/

#include <stdint.h>
#include "ulp_riscv_utils.h"
#include "ulp_riscv_adc_ulp_core.h"
#include "hal/adc_ll.h"
#include "ulp_riscv_gpio.h"

#include "example_config.h"

int32_t wakeup_result;

int main (void)
{
    // ADC
    // uint32_t event = 1;
    // SENS.sar_meas1_ctrl2.sar1_en_pad = 1;
    // while (HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_slave_addr1, meas_status) != 0) {}
    // SENS.sar_meas1_ctrl2.meas1_start_sar = 0;
    // SENS.sar_meas1_ctrl2.meas1_start_sar = 1;
    // while ((bool)SENS.sar_meas1_ctrl2.meas1_done_sar != true) {;}
    // wakeup_result = HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_meas1_ctrl2, meas1_data_sar);
    // ulp_riscv_wakeup_main_processor();


    // GPIO wakeup
    wakeup_result = ulp_riscv_gpio_get_level(GPIO_PIN);
    if (ulp_riscv_gpio_get_level(GPIO_PIN) != wakeup_result) {
        ulp_riscv_wakeup_main_processor();
    }

    return 0;
}
