#include <stdint.h>
#include "renderer.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>


#define COURT_WIDTH 600
#define COURT_HEIGHT 400
#define COURT_X (renderer_screen_width - COURT_WIDTH)/2
#define COURT_Y (renderer_screen_height - COURT_HEIGHT)/2
#define COURT_EDGE_THICKNESS 10

#define NET_THICKNESS 5
#define NET_SEGMENTS 21
#define NET_SEGMENT_LENGTH (COURT_HEIGHT + COURT_EDGE_THICKNESS) / NET_SEGMENTS

#define PLAYER_WIDTH 10
#define PLAYER_HEIGHT 100
#define PLAYER_SPEED 8
#define PLAYER_START_Y COURT_Y + (COURT_HEIGHT / 2) - (PLAYER_HEIGHT / 2)
#define PLAYER_1_START_X COURT_X
#define PLAYER_2_START_X COURT_X + COURT_WIDTH - PLAYER_WIDTH

#define BAT_ANGLE_STEP M_PI / 73

#define BALL_DIAMETER 10
#define BALL_SPEED 10
#define BALL_START_X COURT_X + (COURT_WIDTH / 2) - (BALL_DIAMETER / 2)
#define BALL_START_Y COURT_Y + (COURT_HEIGHT/ 2) - (BALL_DIAMETER / 2)
#define BALL_START_ANGLE_MAX (double)(M_PI / 10)
#define BALL_START_ANGLE_MIN (double)(M_PI / 30)

typedef struct {
  int x;
  int y;
  uint32_t width;
  uint32_t height;
  uint32_t score;
  int v_y;
} player_t;

typedef struct {
  int x;
  int y;
  uint32_t diameter;
  float v_x;
  float v_y;
} ball_t;

typedef enum {
  STATE_START,
  STATE_RUNNING,
  STATE_END
} game_state_t;

const float bat_angles[8] = {-3 * BAT_ANGLE_STEP, -2 *BAT_ANGLE_STEP, -BAT_ANGLE_STEP, 0, 0, BAT_ANGLE_STEP, 2 * BAT_ANGLE_STEP, 3 * BAT_ANGLE_STEP};

player_t player_1;
player_t player_2;
ball_t ball;
char score_text[5];
game_state_t state;

void reset_ball(bool side);

void pong_init(void) {
  player_1 = (player_t){.x = PLAYER_1_START_X, .y = PLAYER_START_Y, .width = PLAYER_WIDTH, .height = PLAYER_HEIGHT, .score = 0, .v_y = 0};
  player_2 = (player_t){.x = PLAYER_2_START_X, .y = PLAYER_START_Y, .width = PLAYER_WIDTH, .height = PLAYER_HEIGHT, .score = 0, .v_y = 0};

  ball = (ball_t){.x = BALL_START_X, .y = BALL_START_Y, .diameter = BALL_DIAMETER, .v_x = 4, .v_y = 3};
  reset_ball(rand() < RAND_MAX / 2);
  state = STATE_START;
  renderer_init();
}

void pong_update(void) {
  ball.x += ball.v_x;
  ball.y += ball.v_y;

  player_1.y += player_1.v_y;
  player_2.y += player_2.v_y;

  // Bat collisions
  if (ball.x <= player_1.x + player_1.width && player_1.y - ball.diameter <= ball.y && ball.y <= player_1.y + player_1.height) {
      ball.x = player_1.x + player_1.width;
    ball.v_x *= -1;
  }

  else if (ball.x >= player_2.x - ball.diameter && player_2.y - ball.diameter <= ball.y && ball.y <= player_2.y + player_2.height) {
    ball.x = player_2.x - ball.diameter;
    ball.v_x *= -1;
  }

  // Court borders
  if (ball.y <= COURT_Y){
    ball.y = COURT_Y;
    ball.v_y = -ball.v_y;
  }
  else if (ball.y >= COURT_Y + COURT_HEIGHT - BALL_DIAMETER){
    ball.y = COURT_Y + COURT_HEIGHT - BALL_DIAMETER;
    ball.v_y = -ball.v_y;
  }
  else if (ball.x < COURT_X) {
    player_2.score++;
    reset_ball(0);
  }
  else if (ball.x > COURT_X + COURT_WIDTH - BALL_DIAMETER) {
    player_1.score++;
    reset_ball(1);
  }
}

void pong_draw(void) {
  renderer_begin_drawing();
  // Court top edge
  renderer_draw_rect(COURT_X - COURT_EDGE_THICKNESS, COURT_Y - COURT_EDGE_THICKNESS, COURT_WIDTH + 2 * COURT_EDGE_THICKNESS, COURT_EDGE_THICKNESS);
  // Court bottom edge
  renderer_draw_rect(COURT_X - COURT_EDGE_THICKNESS, COURT_Y + COURT_HEIGHT, COURT_WIDTH + 2 * COURT_EDGE_THICKNESS, COURT_EDGE_THICKNESS);
  // Court left side
  renderer_draw_rect(COURT_X - COURT_EDGE_THICKNESS, COURT_Y - COURT_EDGE_THICKNESS, COURT_EDGE_THICKNESS, COURT_HEIGHT + 2 * COURT_EDGE_THICKNESS);
  // Court right side
  renderer_draw_rect(COURT_X + COURT_WIDTH, COURT_Y - COURT_EDGE_THICKNESS, COURT_EDGE_THICKNESS, COURT_HEIGHT + 2 * COURT_EDGE_THICKNESS);

  // Net
  for (int i = 0; i < NET_SEGMENTS; i+=2) {
    renderer_draw_rect(COURT_X + ((COURT_WIDTH - NET_THICKNESS) / 2), (COURT_Y - COURT_EDGE_THICKNESS/2) + i * NET_SEGMENT_LENGTH, NET_THICKNESS, NET_SEGMENT_LENGTH);
  }

  renderer_draw_rect(player_1.x, player_1.y, player_1.width, player_1.height);
  renderer_draw_rect(player_2.x, player_2.y, player_2.width, player_2.height);
  renderer_draw_rect(ball.x, ball.y, ball.diameter, ball.diameter);

  sprintf(score_text, "%d", player_1.score);
  renderer_draw_string(COURT_X + COURT_WIDTH / 4, COURT_Y / 2, 2, score_text, strlen(score_text), JUSTIFY_CENTRE);
  sprintf(score_text, "%d", player_2.score);
  renderer_draw_string(COURT_X + 3 * COURT_WIDTH / 4, COURT_Y / 2, 2, score_text, strlen(score_text), JUSTIFY_CENTRE);

  renderer_end_drawing();
}

void pong_move_player(uint32_t player_id, int direction) {
  if (player_id == 1) {
    player_1.v_y = PLAYER_SPEED * (direction % 2);
  }
  else if (player_id == 2) {
    player_2.v_y = PLAYER_SPEED * (direction % 2);
  }
}

void reset_ball(bool side) {
  ball.x = BALL_START_X;
  ball.y = BALL_START_Y;

  double angle = BALL_START_ANGLE_MIN + ((double)rand() / (double)RAND_MAX) * (BALL_START_ANGLE_MAX - BALL_START_ANGLE_MIN);
  if (rand() > RAND_MAX / 2) {
    angle *= -1;
  }

  printf("angle %f\r\n", angle);
  // Start the ball towards the player that scored
  if (side) {
    ball.v_x = -BALL_SPEED * cos(angle);
  }
  else {
    ball.v_x = BALL_SPEED * cos(angle);
  }
  ball.v_y = BALL_SPEED * sin(angle);
}
