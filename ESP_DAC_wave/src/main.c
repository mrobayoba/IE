#include <math.h>

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "soc/dac_channel.h"
#include "driver/dac_continuous.h"

#include "freertos/FreeRTOS.h"

#include "esp_check.h"

/**
 *  There are two ways to convert digital data to analog signal continuously:
 *  - Using a timer: setting DAC voltage periodically in the timer interrupt
 *                   in this way, DAC can achieve a relatively low conversion frequency
 *                   but it is not a efficient way comparing to using the DMA
 *  - Using DMA: tansmitting the data buffer via DMA,
 *               the conversion frequency is controlled by how fast it is transmitted by DMA
 *               in this way, the conversion frequency can reach several MHz,
 *               but it can't achieve a very low conversion frequency because it is limited by the DMA clock source
 *  Generally, recommand to use DMA, if the DMA peripheral is occupied or the required conversion frequency is very low,
 *  then use timer instead
 */

#define DAC_AMPLITUDE 255
#define PI 3.14159

/**
 *ADJUST THIS PARAMETER AT LEAST 10 TIMES GREATER THAN SIGNAL FREQ TO READ
 */
#define DELAY_US 100

dac_continuous_handle_t cont_handle;

const float carrierfreq = 1000; //  carrier wave  freq in Hz
const float modFreq = 100;      // Modulated signal freq in Hz ( FOR ESP32 KEEP THIS FREQ ABOVE 648Hz)

const int sampling_rate = 700; // same as analogWaveMod
const float convert_freq = modFreq * sampling_rate;  // BE SURE TO NOT EXCEED 2MHz
float t_delta = 1.0 / convert_freq; // Time step to get samples

float A_c = 1.0; // Amplitude of the carrier
float A_m = 0.8; // Amplitude of the modulated

_Static_assert(DAC_AMPLITUDE < 256, "The DAC accuracy is 8 bit-width, doesn't support the amplitude beyond 255");

static void generate_wave(uint8_t *array)
{
    float t = 0;
    float c = 0; // To hold carrier values
    float m = 0; // To hold modulated values
    float s = 0; // To hold AM signal values
    float A_max = A_c + A_m;

    for (int i = 0; i < sampling_rate; i++)
    {
        c = A_c * cos(2 * PI * carrierfreq * t);
        m = A_m * cos(2 * PI * modFreq * t);

        s = (1 + m) * c + A_max; // AM signal sample
        s = s / (2 * A_max);     // Normalized AM signal sample
        s = (s * 1000 - 0) * (DAC_AMPLITUDE - 70) / (1000 - 0) + 70;
        array[i] = (uint8_t)s; // Stores samples into the buffer

        t = t + t_delta;
    }
}

/* General states*/
#define LOW 0
#define HIGH 1
#define RESET LOW
#define SET HIGH

/* Peripherals Handlers*/
adc_oneshot_unit_handle_t adc2_handler;

/*Callbacks*/
/* General Functions*/
static double ADC_conversion(int rawVal){
    return (double) rawVal * 3300.0 / 1024;
}

void initSys(void);

void app_main(void)
{
    int rawData = 0; // For ADC data acquisition

    uint8_t waveSamples[sampling_rate]; // Used to store AM wave values

    generate_wave(waveSamples);

    initSys();

    printf("Initialized.\n");
    
    /* *
     * The wave in the buffer will be converted cyclically, there is no need to manage this function.
     * */
        ESP_ERROR_CHECK(dac_continuous_write_cyclically(cont_handle, waveSamples, (size_t) sampling_rate-1, NULL));

    /**
     * Infinite loop
     */
    while(1){
        // For ADC readings and serial data transmission
        adc_oneshot_read(adc2_handler,ADC_CHANNEL_4,&rawData);
        printf("%4.2f\n",ADC_conversion(rawData));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void initSys(void)
{
    /*config DACs*/
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_CH0,
        .desc_num = 8,
        .buf_size = 2048,
        .freq_hz = convert_freq,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_APLL, // If the frequency is out of range, try 'DAC_DIGI_CLK_SRC_APLL'
    };
    /* Allocate continuous channel */
    ESP_ERROR_CHECK(dac_continuous_new_channels(&cont_cfg, &cont_handle));
    /* Enable the channels in the group */
    ESP_ERROR_CHECK(dac_continuous_enable(cont_handle));

    /*config ADCs*/
    /**
     * Look for pinout before switching GPIO for ADC reading
     * (ADC_UNIT_2, ADC_CHANNEL_4) = GPIO13
     */
    adc_oneshot_unit_init_cfg_t adcConfig = {
        .unit_id = ADC_UNIT_2,
        .clk_src = ADC_RTC_CLK_SRC_DEFAULT,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adcConfig,&adc2_handler)); 

    adc_oneshot_chan_cfg_t adcChannelConfig = {
        .atten = ADC_ATTEN_DB_12, // 3V3 max input voltage
        .bitwidth = ADC_BITWIDTH_10, // 10-bit resolution (~3mV)
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handler,ADC_CHANNEL_4, &adcChannelConfig));
}