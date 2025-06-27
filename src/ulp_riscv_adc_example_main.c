#include <stdio.h>
#include <inttypes.h>
#include "esp_sleep.h"
#include "ulp_riscv.h"
#include "ulp_adc.h"
#include "ulp_main.h"
#include "ulp/example_config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "driver/adc.h"
#include "esp_ws28xx.h"
#include "esp_adc/adc_oneshot.h"

#define LED_GPIO 48

extern const uint8_t ulp_main_bin_start[] asm("_binary_ulp_main_bin_start");
extern const uint8_t ulp_main_bin_end[]   asm("_binary_ulp_main_bin_end");

static void init_ulp_program(void);

CRGB* ws2812_buffer;

void flash_led(int r, int g, int b, bool wait) {
    ws2812_buffer[0] = (CRGB){.r = r, .g = g, .b = b};
    ESP_ERROR_CHECK_WITHOUT_ABORT(ws28xx_update());
    if (wait) vTaskDelay(pdMS_TO_TICKS(1000));
}

void app_main(void)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(ws28xx_init(LED_GPIO, WS2812B, 1, &ws2812_buffer));
    flash_led(255, 0, 0, true);
    // flash_led(0, 255, 0, true);
    // flash_led(0, 0, 255, true);
    flash_led(0, 0, 0, false);
    
    esp_sleep_wakeup_cause_t cause = esp_sleep_get_wakeup_cause();

    /* not a wakeup from ULP, load the firmware */
    if ((cause != ESP_SLEEP_WAKEUP_ULP) && (cause != ESP_SLEEP_WAKEUP_TIMER)) {
        printf("Not a ULP-RISC-V wakeup (cause = %d), initializing it! \n", cause);
        init_ulp_program();
    }

    /* ULP Risc-V read and detected a temperature above the limit */
    if (cause == ESP_SLEEP_WAKEUP_ULP) {
        printf("ULP-RISC-V woke up the main CPU\n");
        printf("Value = %"PRIu32" was above threshold\n", ulp_wakeup_result);
    }

    /* Go back to sleep, only the ULP Risc-V will run */
    printf("Entering in deep sleep\n\n");

    /* RTC peripheral power domain needs to be kept on to keep SAR ADC related configs during sleep */
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

    ESP_ERROR_CHECK( esp_sleep_enable_ulp_wakeup());

    esp_deep_sleep_start();
    // esp_light_sleep_start();
}

static void init_ulp_program(void)
{

    // GPIO wakeup
    rtc_gpio_init(GPIO_PIN);
    rtc_gpio_set_direction(GPIO_NUM_0, RTC_GPIO_MODE_INPUT_ONLY);
    rtc_gpio_pulldown_dis(GPIO_PIN);
    rtc_gpio_pullup_dis(GPIO_PIN);
    rtc_gpio_hold_en(GPIO_PIN);


    // new
    // adc_oneshot_unit_handle_t adc1_handle;
    // adc_oneshot_unit_init_cfg_t init_config1 = {
    //     .unit_id = ADC_UNIT_1,
    //     .ulp_mode = ADC_ULP_MODE_RISCV,
    // };
    // ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));
    // adc_oneshot_chan_cfg_t config = {
    //     .atten = EXAMPLE_ADC_ATTEN,
    //     .bitwidth = ADC_BITWIDTH_9,
    // };
    // ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, 0, &config));
    
    // // legacy
    // adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_11);
    // adc1_config_width(ADC_WIDTH_BIT_12);
    // adc1_ulp_enable();

    // ulp_adc_cfg_t cfg = {
    //     .adc_n    = EXAMPLE_ADC_UNIT,
    //     .channel  = EXAMPLE_ADC_CHANNEL,
    //     .width    = EXAMPLE_ADC_WIDTH,
    //     .atten    = EXAMPLE_ADC_ATTEN,
    //     .ulp_mode = ADC_ULP_MODE_RISCV,
    // };

    // ESP_ERROR_CHECK(ulp_adc_init(&cfg));

    esp_err_t err = ulp_riscv_load_binary(ulp_main_bin_start, (ulp_main_bin_end - ulp_main_bin_start));
    ESP_ERROR_CHECK(err);

    /* The first argument is the period index, which is not used by the ULP-RISC-V timer
     * The second argument is the period in microseconds, which gives a wakeup time period of: 20ms
     */
    ulp_set_wakeup_period(0, 20000);

    /* Start the program */
    err = ulp_riscv_run();
    ESP_ERROR_CHECK(err);
}
