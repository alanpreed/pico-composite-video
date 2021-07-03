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

const uint8_t pix_per_line = 52 * 4;

void handle_pio(void) {
  irq_clear(PIO0_IRQ_0);
  pio_interrupt_clear(pio0, 0);

  uint32_t *line = buffer_fetch_line();

  for (int i = 0; i < CVIDEO_PIX_PER_LINE / 32; i++) {
    pio_sm_put(pio0, 0, line[i]);
  }
}

int main()
{
    // Defaults: UART 0, TX pin 0, RX pin 1, baud rate 115200
    buffer_init();
    buffer_draw_rect(5, 300, 2, 40);
    buffer_draw_rect(10, 300, 2, 40);

    buffer_draw_rect(5, 410, 2, 40);
    buffer_draw_rect(10, 410, 2, 40);
    buffer_draw_rect(5, 450, 7, 40);

    stdio_init_all();
    gpio_set_function(PICO_DEFAULT_UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(PICO_DEFAULT_UART_RX_PIN, GPIO_FUNC_UART);


    puts("Hello, world!");

  // pio_interrupt_source
    irq_set_enabled(PIO0_IRQ_0, true);
    irq_set_exclusive_handler(PIO0_IRQ_0, handle_pio);
    pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS;

    cvideo_init(pio0, cvideo_data_pin, cvideo_sync_pin, CVIDEO_PIX_PER_LINE);

    printf("Test\r\n");
    printf("Clock speed %d\r\n", clock_get_hz(clk_sys));

    while(1);

    return 0;
}
