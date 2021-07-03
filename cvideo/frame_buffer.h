#include "cvideo.h"

#ifndef __FRAME_BUFFER_H__
#define __FRAME_BUFFER_H__

extern uint32_t frame_buffer[CVIDEO_LINES][CVIDEO_PIX_PER_LINE / 32];
extern volatile int current_line;

void buffer_init(void);

void buffer_draw_rect(uint x, uint y, uint width, uint height);

uint32_t* buffer_fetch_line(void);

#endif