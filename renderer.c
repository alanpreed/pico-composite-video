#include "renderer.h"
#include "cvideo.h"
#include "connections.h"
#include <stdio.h>
#include <stdbool.h>
#include "font.xbm"

#define LINE_WORD_COUNT CVIDEO_PIX_PER_LINE / 32

#define CHAR_WIDTH font_width / 16
#define CHAR_HEIGHT font_height / 16

typedef uint32_t buffer_t[CVIDEO_LINES][LINE_WORD_COUNT];

uint32_t renderer_screen_width = CVIDEO_PIX_PER_LINE;
uint32_t renderer_screen_height = CVIDEO_LINES;
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

bool was_empty = false;

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
  if (pio_sm_is_tx_fifo_empty(pio0, 0)) {
      was_empty = true;
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
  uint32_t x_word_start_index = x / 32;
  uint32_t x_word_end_index = (x + width) / 32;

  uint32_t x_word_start_boundary_offset = x % 32;
  uint32_t x_word_end_boundary_offset = (x + width) % 32;

  // Set first and last words manually
  // Set other words quickly

  for (int j = y; j < y + height; j++) {
    // Small rectangles sometimes fit entirely within one word
    if (x_word_start_index == x_word_end_index) {
      uint32_t data = 0xFFFFFFFF;
      data ^= 0xFFFFFFFF << x_word_start_boundary_offset;
      data ^= 0xFFFFFFFF >> (32 - x_word_end_boundary_offset);
      (*drawing_buffer)[j][x_word_start_index] |= data;
    }
    else {
      // First word and last word only partially filled
      (*drawing_buffer)[j][x_word_start_index] |= 0xFFFFFFFF << x_word_start_boundary_offset;
      (*drawing_buffer)[j][x_word_end_index] |= 0xFFFFFFFF >> (32 - x_word_end_boundary_offset);

      for (int i = x_word_start_index + 1; i < x_word_end_index; i++) {
        (*drawing_buffer)[j][i] = 0xFFFFFFFF;
      }
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

void renderer_draw_character(unsigned int x, unsigned int y, unsigned int scale, char character) {
  uint8_t row = character / 16;
  uint8_t column = character % 16;
  uint32_t font_x = column * 10;
  uint32_t font_y = row * 12;

  for (int j = font_y; j < font_y + CHAR_HEIGHT; j++) {
    for  (int i = font_x; i <  font_x + CHAR_WIDTH; i++) {
      // XBM images pad rows with zeros
      uint32_t array_position;
      if (font_width %8 != 0) {
        array_position = (j * (font_width + (8 - font_width % 8))) + i;
      }
      else {
        array_position = (j * font_width) + i;
      }
      uint32_t array_index = array_position / 8;
      // XBM image, low bits are first
      uint32_t byte_position = array_position % 8;

      uint8_t value = font_bits[array_index] >> byte_position & 1 ;

      for (int sx = 0; sx < scale; sx++) {
        for (int sy = 0; sy < scale; sy++) {
          set_bit(x + ((i - font_x) * scale) + sx, y + ((j- font_y) * scale) + sy, !value);
        }
      }
    }
  }
}

void renderer_draw_string(unsigned int x, unsigned int y, unsigned int scale, char *text, unsigned int length, renderer_text_justify_t justification){
  switch (justification) {
    case JUSTIFY_LEFT:
      for(int i = 0; i < length; i++) {
        renderer_draw_character(x + (i * CHAR_WIDTH * scale), y, scale, text[i]);
      }
      break;

    case JUSTIFY_CENTRE:
      for(int i = 0; i < length; i++) {
        renderer_draw_character(x - (length * scale * CHAR_WIDTH / 2) + (i * CHAR_WIDTH * scale), y, scale, text[i]);
      }
      break;

    case JUSTIFY_RIGHT:
      for (int i = 0; i < length; i++) {
        renderer_draw_character(x - ((length - i) * CHAR_WIDTH * scale), y, scale, text[i]);
      }
      break;

    default:
      printf("Invalid justification\r\n");
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
