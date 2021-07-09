#include "renderer.h"
#include "cvideo.h"
#include "connections.h"

uint32_t frame_buffer[CVIDEO_LINES][CVIDEO_PIX_PER_LINE / 32];
volatile int current_line;

static void set_bit(uint x, uint y);

uint32_t data_callback(void) {
  return 0xAAAA0000;
}

void renderer_init(void) {
  for (int i =0; i < CVIDEO_LINES; i++) {
    for (int j = 0; j < CVIDEO_PIX_PER_LINE / 32; j++) {
      frame_buffer[i][j] = 0;
    }
  }
  current_line = 0;

  cvideo_init(pio0, CVIDEO_DATA_PIN, CVIDEO_SYNC_PIN, data_callback);
}


void renderer_draw_rect(uint x, uint y, uint width, uint height) {

  frame_buffer[y][0] = 0xaaaaFFFF;//0xAAAA0000;

  // frame_buffer[y - 10][1] = 0x0000AAAA;
  // for (int i = x; i < x + width; i++) {
  //   for (int j = y; j < y + height; j++) {
  //     set_bit(i, j);
  //   }
  // }
   
}


uint32_t* buffer_fetch_line(void) {
  uint32_t *line = frame_buffer[current_line];
  current_line = current_line + 2;
  
  if (current_line == CVIDEO_LINES + 1){
    current_line = 0;
  }
  else if (current_line == CVIDEO_LINES){
    current_line = 1;
  }

  return line;
}

static void set_bit(uint x, uint y) {
  uint index_x = x / 32;
  uint pos_x = x % 32;

  uint flag = 1 << (31 - pos_x);
  frame_buffer[y][index_x] |= flag;
}