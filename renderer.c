#include "renderer.h"
#include "cvideo.h"
#include "connections.h"
#include <stdio.h>
#include <stdbool.h>

#define LINE_WORD_COUNT CVIDEO_PIX_PER_LINE / 32
volatile int current_line;
volatile int current_pix;

uint32_t frame_buffer[CVIDEO_LINES][LINE_WORD_COUNT];

static void set_bit(uint x, uint y, bool value);

uint32_t data_callback(void) {
  uint32_t *line = frame_buffer[current_line];
  uint32_t data = line[current_pix];

  current_pix++;

  if (current_pix == LINE_WORD_COUNT) {
    current_pix = 0;
    current_line = current_line + 2;

    if (current_line == CVIDEO_LINES + 1){
      current_line = 0;
    }
    else if (current_line == CVIDEO_LINES){
      current_line = 1;
    }
  }

  return data;
}

void renderer_init(void) {
  for (int i =0; i < CVIDEO_LINES; i++) {
    for (int j = 0; j < CVIDEO_PIX_PER_LINE / 32; j++) {
      frame_buffer[i][j] = 0;
    }
  }

  // PIO starts with odd lines
  current_line = 1;
  current_pix = 0;

  cvideo_init(pio0, CVIDEO_DATA_PIN, CVIDEO_SYNC_PIN, data_callback);
}


void renderer_draw_rect(uint x, uint y, uint width, uint height) {
  for (int i = x; i < x + width; i++) {
    for (int j = y; j < y + height; j++) {
      set_bit(i, j, true);
    }
  }
}

void renderer_draw_image(unsigned int x, unsigned int y, unsigned int width, unsigned int height, char *data) {
  for (int j = 0; j < height; j++) {
    for  (int i = 0; i < width; i++) {
      // XBM images pad rows with zeros
      uint32_t array_position = (j * (width + (8 - width % 8))) + i;
      uint32_t array_index = array_position / 8;
      // XBM image, low bits are first
      uint32_t byte_position = array_position % 8;

      uint8_t value = data[array_index] >> byte_position & 1 ;//(1 << (7 - byte_position));

      set_bit(x + i, y + j, !value);
    }
  }
}


static void set_bit(uint x, uint y, bool value) {
  uint index_x = x / 32;
  uint pos_x = x % 32;

  uint flag = value << (31 - pos_x);
  frame_buffer[y][index_x] |= flag;
}