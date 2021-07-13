#ifndef __PONG_H__
#define __PONG_H__

#include <stdint.h>

#define PONG_FRAME_INTERVAL_ms 10

typedef enum {
  PONG_DIRECTION_UP = -1,
  PONG_DIRECTION_STOP = 0,
  PONG_DIRECTION_DOWN = 1,
} pong_player_direction_t;

void pong_init(void);

void pong_update(void);

void pong_tick(void);

void pong_move_player(uint32_t player_id, pong_player_direction_t direction);

#endif
