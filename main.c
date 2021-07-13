#include <stdio.h>
#include "pico/stdlib.h"
#include <stdlib.h>

#include "hardware/timer.h"
#include "hardware/gpio.h"
#include "connections.h"
#include "pong.h"


bool pong_gametick_callback(struct repeating_timer* t) {
  pong_tick();
  return true;
}

void button_cb(unsigned int gpio, uint32_t events) {
  if (events & GPIO_IRQ_EDGE_RISE) { 
    if (gpio == PLAYER1_BUTTON_UP || gpio == PLAYER1_BUTTON_DOWN) {
      pong_move_player(1, PONG_DIRECTION_STOP);
    }
    else if (gpio == PLAYER2_BUTTON_UP || gpio == PLAYER2_BUTTON_DOWN) {
      pong_move_player(2, PONG_DIRECTION_STOP);
    }
  } else if (events & GPIO_IRQ_EDGE_FALL) { 
    switch(gpio) {
      case PLAYER1_BUTTON_UP:
        pong_move_player(1, PONG_DIRECTION_UP);
        break;
      case PLAYER1_BUTTON_DOWN:
        pong_move_player(1, PONG_DIRECTION_DOWN);
        break;
      case PLAYER2_BUTTON_UP:
        pong_move_player(2, PONG_DIRECTION_UP);
        break;
      case PLAYER2_BUTTON_DOWN:
        pong_move_player(2, PONG_DIRECTION_DOWN);
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

  struct repeating_timer timer;
  add_repeating_timer_ms(PONG_FRAME_INTERVAL_ms, pong_gametick_callback, NULL, &timer);

  gpio_pull_up(PLAYER1_BUTTON_DOWN);
  gpio_pull_up(PLAYER1_BUTTON_UP);
  gpio_pull_up(PLAYER2_BUTTON_DOWN);
  gpio_pull_up(PLAYER2_BUTTON_UP);
  gpio_set_irq_enabled_with_callback(PLAYER1_BUTTON_DOWN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_cb);
  gpio_set_irq_enabled_with_callback(PLAYER1_BUTTON_UP, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_cb);
  gpio_set_irq_enabled_with_callback(PLAYER2_BUTTON_DOWN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_cb);
  gpio_set_irq_enabled_with_callback(PLAYER2_BUTTON_UP, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &button_cb);

  while(1) {
    pong_update();
  }

  return 0;
}
