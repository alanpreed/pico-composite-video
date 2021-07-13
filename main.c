#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>

#include "hardware/dma.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "renderer.h"
#include "math.h"
// #include "renderer.h"
#include "connections.h"
#include "pong.h"
#include "test.xbm"

bool redraw = false;
uint32_t xpos = 0;

bool repeating_timer_callback(struct repeating_timer* t) {
    // printf("Repeat at %lld\n",time_us_64());
    redraw = true;


    return true;
}

void button_cb(unsigned int gpio, uint32_t events) {
  if (events & GPIO_IRQ_EDGE_RISE) { 
    if (gpio == PLAYER1_BUTTON_UP || gpio == PLAYER1_BUTTON_DOWN) {
      pong_move_player(1, 0);
    }
    else if (gpio == PLAYER2_BUTTON_UP || gpio == PLAYER2_BUTTON_DOWN) {
      pong_move_player(2, 0);
    }
  } else if (events & GPIO_IRQ_EDGE_FALL) { 
    switch(gpio) {
      case PLAYER1_BUTTON_UP:
        printf("p1 up \r\n");
        pong_move_player(1, -1);
        break;
      case PLAYER1_BUTTON_DOWN:
        printf("p1 down \r\n");
        pong_move_player(1, 1);
        break;
      case PLAYER2_BUTTON_UP:
        printf("p2 up \r\n");
        pong_move_player(2, -1);
        break;
      case PLAYER2_BUTTON_DOWN:
        printf("p2 down \r\n");
        pong_move_player(2, 1);
        break;
    }
  }
}

int main()
{
  // Defaults: UART 0, TX pin 0, RX pin 1, baud rate 115200
  stdio_init_all();
  gpio_set_function(SERIAL_RX_PIN, GPIO_FUNC_UART);
  gpio_set_function(SERIAL_TX_PIN, GPIO_FUNC_UART);

  puts("Hello, world!");
  pong_init();

  printf("Test\r\n");
  printf("Clock speed %d\r\n", clock_get_hz(clk_sys));

  struct repeating_timer timer;
  add_repeating_timer_ms(100, repeating_timer_callback, NULL, &timer);

  gpio_pull_up(PLAYER1_BUTTON_DOWN);
  gpio_pull_up(PLAYER1_BUTTON_UP);
  gpio_pull_up(PLAYER2_BUTTON_DOWN);
  gpio_pull_up(PLAYER2_BUTTON_UP);
  gpio_set_irq_enabled_with_callback(PLAYER1_BUTTON_DOWN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_cb);
  gpio_set_irq_enabled_with_callback(PLAYER1_BUTTON_UP, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_cb);
  gpio_set_irq_enabled_with_callback(PLAYER2_BUTTON_DOWN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_cb);
  gpio_set_irq_enabled_with_callback(PLAYER2_BUTTON_UP, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_cb);

  while(1) {
    if (was_empty) {
      was_empty = false;
      printf("Data empty\r\n");
    }

    if (redraw) {
        uint32_t time_before = time_us_32();
        // printf("%d\r\n", rand());
        pong_update();
        pong_draw();
        uint32_t time_after = time_us_32();
        redraw = false;
        xpos++;
        // printf("drawing time %d \r\n", time_after - time_before);
        // while(1);
    }
  }

  return 0;
}
