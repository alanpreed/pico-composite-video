#include "frame_buffer.h"

volatile int current_line;
uint32_t frame_buffer[CVIDEO_LINES][CVIDEO_PIX_PER_LINE / 32];

static void set_bit(uint x, uint y) {
  uint index_x = x / 32;
  uint pos_x = x % 32;

  uint flag = 1 << (31 - pos_x);
  frame_buffer[y][index_x] |= flag;

}

void buffer_init(void) {
  for (int i =0; i < CVIDEO_LINES; i++) {
    for (int j = 0; j < CVIDEO_PIX_PER_LINE / 32; j++) {
      frame_buffer[i][j] = 0;
    }
  }
  current_line = 0;
}


void buffer_draw_rect(uint x, uint y, uint width, uint height) {
  for (int i = x; i < x + width; i++) {
    for (int j = y; j < y + height; j++) {
      set_bit(i, j);
    }
  }
}


uint32_t* buffer_fetch_line(void) {

  uint32_t *line = frame_buffer[current_line];

  current_line = (current_line + 2) % CVIDEO_LINES;
  return line;
}