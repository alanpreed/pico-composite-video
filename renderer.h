#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <stdint.h>

void renderer_init(void);

void renderer_draw_rect(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

#endif