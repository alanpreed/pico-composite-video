#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
#include "cvideo/cvideo.h"


int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}

const uint8_t cvideo_data_pin = 2;
const uint8_t cvideo_sync_pin = 3;


int main()
{
    // Defaults: UART 0, TX pin 0, RX pin 1, baud rate 115200
    stdio_init_all();
    gpio_set_function(PICO_DEFAULT_UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(PICO_DEFAULT_UART_RX_PIN, GPIO_FUNC_UART);

    // Timer example code - This example fires off the callback after 2000ms
    add_alarm_in_ms(2000, alarm_callback, NULL, false);

    puts("Hello, world!");

    cvideo_init(pio0, cvideo_data_pin, cvideo_sync_pin);

    printf("Test\r\n");

    while(1);

    return 0;
}
