#ifndef __PONG_H__
#define __PONG_H__

void pong_init(void);

void pong_draw(void);

void pong_update(void);

void pong_move_player(uint32_t player_id, int direction);

#endif
