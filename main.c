#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
#include "cvideo/cvideo.h"
#include "hardware/irq.h"
#include "cvideo/frame_buffer.h"


const uint8_t cvideo_data_pin = 2;
const uint8_t cvideo_sync_pin = 3;

const uint8_t int_probe_pin = 4;

uint32_t data_callback(void) {
  return 0xAAAA0000;
}

int main()
{
    // Defaults: UART 0, TX pin 0, RX pin 1, baud rate 115200
    // buffer_init();
    // // buffer_draw_rect(5, 300, 1, 40);
    // // buffer_draw_rect(10, 300, 1, 40);

    // // buffer_draw_rect(5, 410, 1, 40);
    // // buffer_draw_rect(10, 410, 1, 40);
    // // buffer_draw_rect(5, 450, 6, 40);

    // buffer_draw_rect(0, 450, 29, 1);

    stdio_init_all();
    gpio_init(int_probe_pin);
    gpio_set_function(PICO_DEFAULT_UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(PICO_DEFAULT_UART_RX_PIN, GPIO_FUNC_UART);
    gpio_set_dir(int_probe_pin, true);
    gpio_put(int_probe_pin, 0);


    puts("Hello, world!");

    cvideo_init(pio0, cvideo_data_pin, cvideo_sync_pin, data_callback);

    printf("Test\r\n");
    printf("Clock speed %d\r\n", clock_get_hz(clk_sys));

    while(1);

    return 0;
}
