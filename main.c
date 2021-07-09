#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/dma.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#include "renderer.h"
#include "connections.h"


int main()
{
    // Defaults: UART 0, TX pin 0, RX pin 1, baud rate 115200
    // // buffer_draw_rect(5, 300, 1, 40);
    // // buffer_draw_rect(10, 300, 1, 40);

    // // buffer_draw_rect(5, 410, 1, 40);
    // // buffer_draw_rect(10, 410, 1, 40);
    // // buffer_draw_rect(5, 450, 6, 40);

    // buffer_draw_rect(0, 450, 29, 1);

    stdio_init_all();
    gpio_set_function(SERIAL_RX_PIN, GPIO_FUNC_UART);
    gpio_set_function(SERIAL_TX_PIN, GPIO_FUNC_UART);

    puts("Hello, world!");
    renderer_init();

    printf("Test\r\n");
    printf("Clock speed %d\r\n", clock_get_hz(clk_sys));

    while(1);

    return 0;
}
