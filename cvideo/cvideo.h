#include "hardware/pio.h"

#ifndef __CVIDEO_H__
#define __CVIDEO_H__

#define CVIDEO_LINES 576
#define CVIDEO_PIX_PER_LINE 32
//52 * 10

void cvideo_init(PIO pio, uint data_pin, uint sync_pin, uint bits_per_line); 

#endif