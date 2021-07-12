#ifndef __CONNECTIONS_H__
#define __CONNECTIONS_H__

#include "pico.h"

// Composite video output
#define CVIDEO_DATA_PIN 2
#define CVIDEO_SYNC_PIN 3

// Serial output
#define SERIAL_RX_PIN PICO_DEFAULT_UART_RX_PIN
#define SERIAL_TX_PIN PICO_DEFAULT_UART_TX_PIN

// Buttons
#define PLAYER1_BUTTON_UP 10
#define PLAYER1_BUTTON_DOWN 11
#define PLAYER2_BUTTON_UP 12
#define PLAYER2_BUTTON_DOWN 13

#endif
