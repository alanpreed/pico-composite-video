#include "renderer.h"
#include "cvideo.h"
#include "connections.h"
#include <stdio.h>
#include <stdbool.h>

#define LINE_WORD_COUNT CVIDEO_PIX_PER_LINE / 32

typedef uint32_t buffer_t[CVIDEO_LINES][LINE_WORD_COUNT];

volatile int current_line;
volatile int current_pix;
bool drawing_in_progress;
bool buffer_updated;
buffer_t buffer_0;
buffer_t buffer_1;
buffer_t *output_buffer = &buffer_0;
buffer_t *drawing_buffer = &buffer_1;

static void set_bit(uint x, uint y, bool value);
static void renderer_clear(buffer_t *buffer);
static void update_output_buffer(void);

uint32_t data_callback(void) {
  uint32_t *line = (*output_buffer)[current_line];
  uint32_t data = line[current_pix];

  current_pix++;

  if (current_pix == LINE_WORD_COUNT) {
    current_pix = 0;
    current_line = current_line + 2;

    if (current_line == CVIDEO_LINES + 1){
      current_line = 0;
      update_output_buffer();
    }
    else if (current_line == CVIDEO_LINES){
      current_line = 1;
      update_output_buffer();
    }
  }

  return data;
}

void renderer_init(void) {
  renderer_clear(output_buffer);
  renderer_clear(drawing_buffer);
  drawing_in_progress = false;
  buffer_updated = false;

  // PIO starts with odd lines
  current_line = 1;
  current_pix = 0;

  cvideo_init(pio0, CVIDEO_DATA_PIN, CVIDEO_SYNC_PIN, data_callback);
}

void renderer_begin_drawing(void) {
  // CVIDEO uses output_buffer for output
  // Renderer draws to frame_buffer
  drawing_in_progress = true;
  renderer_clear(drawing_buffer);
}

void renderer_end_drawing(void) {
  drawing_in_progress = false;
  buffer_updated = true;
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

static void update_output_buffer(void){
  if(!drawing_in_progress && buffer_updated){
    // Swap drawing and output buffer
    buffer_t *temp = output_buffer;
    output_buffer = drawing_buffer;
    drawing_buffer = temp;
    buffer_updated = false;
  }
}

static void renderer_clear(buffer_t *buffer) {
  for (int i =0; i < CVIDEO_LINES; i++) {
    for (int j = 0; j < CVIDEO_PIX_PER_LINE / 32; j++) {
      (*buffer)[i][j] = 0;
    }
  }
}

static void set_bit(uint x, uint y, bool value) {
  uint index_x = x / 32;
  uint pos_x = x % 32;

  uint flag = value << pos_x;
  (*drawing_buffer)[y][index_x] |= flag;
}