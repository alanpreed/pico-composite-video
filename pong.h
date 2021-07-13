#ifndef __PONG_H__
#define __PONG_H__

#define PONG_FRAME_INTERVAL_ms 10

void pong_init(void);

void pong_draw(void);

void pong_update(void);

void pong_move_player(uint32_t player_id, int direction);

#endif
