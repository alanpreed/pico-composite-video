#include "hardware/pio.h"

#ifndef __CVIDEO_H__
#define __CVIDEO_H__

#define CVIDEO_LINES 576
#define CVIDEO_PIX_PER_LINE 64

typedef uint32_t (*cvideo_data_callback_t)(void); 

void cvideo_init(PIO pio, uint data_pin, uint sync_pin, cvideo_data_callback_t callback); 

#endif