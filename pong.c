#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "pong.h"
#include "vec2.h"
#include "renderer.h"

#define COURT_WIDTH 600
#define COURT_HEIGHT 400
#define COURT_EDGE_THICKNESS 10
#define COURT_X (renderer_screen_width - COURT_WIDTH)/2
#define COURT_Y (renderer_screen_height - COURT_HEIGHT)/2

#define NET_THICKNESS 5
#define NET_SEGMENTS 21
#define NET_SEGMENT_LENGTH (COURT_HEIGHT + COURT_EDGE_THICKNESS) / NET_SEGMENTS

#define PLAYER_WIN_THRESHOLD 11
#define PLAYER_WIDTH 10
#define PLAYER_HEIGHT 100
#define PLAYER_SPEED 8
#define PLAYER_START_Y COURT_Y + (COURT_HEIGHT / 2) - (PLAYER_HEIGHT / 2)
#define PLAYER_Y_MIN COURT_Y
#define PLAYER_Y_MAX COURT_Y + COURT_HEIGHT - PLAYER_HEIGHT
#define PLAYER_1_START_X COURT_X
#define PLAYER_2_START_X COURT_X + COURT_WIDTH - PLAYER_WIDTH

#define BAT_ANGLE_STEP M_PI / 64
#define BAT_DIVSIONS 8

#define BALL_DIAMETER 10
#define BALL_START_SPEED 2
#define BALL_ACCELERATION 0.5
#define BALL_START_ANGLE_MAX (double)(M_PI / 10)
#define BALL_START_ANGLE_MIN (double)(M_PI / 30)
#define BALL_START_X COURT_X + (COURT_WIDTH / 2) - (BALL_DIAMETER / 2)
#define BALL_START_Y COURT_Y + (COURT_HEIGHT/ 2) - (BALL_DIAMETER / 2)

typedef struct {
  int id;
  vec2_t position;
  vec2_t velocity;
  uint32_t width;
  uint32_t height;
  uint32_t score;
} player_t;

typedef struct {
  vec2_t position;
  vec2_t velocity;
  uint32_t diameter;
} ball_t;

typedef enum {
  STATE_START,
  STATE_RUNNING,
  STATE_WIN,
} game_state_t;

// Bat is not actually flat, instead divided into 8 sections with different angles
// following info on Wikipedia (https://en.wikipedia.org/wiki/Pong#Development_and_history)
static const float bat_angles[8] = {-3 * BAT_ANGLE_STEP, -2 *BAT_ANGLE_STEP, -BAT_ANGLE_STEP, 0, 0, BAT_ANGLE_STEP, 2 * BAT_ANGLE_STEP, 3 * BAT_ANGLE_STEP};
static player_t player_1;
static player_t player_2;
static ball_t ball;
static char score_text[5];
static game_state_t state;
static bool update_game_flag;

static void draw(void);
static void reset_ball(int player_id);
static void bounce_ball(float surface_angle);
static void update_player_position(player_t *player);
static void reset_game(void); 
static void score_point(player_t *player);

void pong_tick(void) {
  update_game_flag = true;
}

void pong_init(void) {
  reset_game();
  renderer_init(draw);
}

void pong_update(void) {
  renderer_run();
  if(update_game_flag) {
    update_game_flag = false;
    if (state == STATE_RUNNING) {
      ball.position = vec2_add(ball.position, ball.velocity);
      update_player_position(&player_1);
      update_player_position(&player_2);

      // Bat collisions, with bats divided into angled segments
      if (ball.position.v0 <= player_1.position.v0 + player_1.width && player_1.position.v1 - ball.diameter <= ball.position.v1 && ball.position.v1 <= player_1.position.v1 + player_1.height) {
        ball.position.v0 = player_1.position.v0 + player_1.width;

        uint32_t bat_index_position = (ball.position.v1 - player_1.position.v1) / (PLAYER_HEIGHT / BAT_DIVSIONS);
        bounce_ball(bat_angles[bat_index_position]);
      }
      else if (ball.position.v0 >= player_2.position.v0 - ball.diameter && player_2.position.v1 - ball.diameter <= ball.position.v1 && ball.position.v1 <= player_2.position.v1 + player_2.height) {
        ball.position.v0 = player_2.position.v0 - ball.diameter;

        uint32_t bat_index_position = BAT_DIVSIONS - (ball.position.v1 - player_2.position.v1) / (PLAYER_HEIGHT / BAT_DIVSIONS);
        bounce_ball(bat_angles[bat_index_position]);
      }

      // Court borders - simple v_y reflection on top/bottom, scoring on left/right
      if (ball.position.v1 <= COURT_Y){
        ball.position.v1 = COURT_Y;
        ball.velocity.v1 = -ball.velocity.v1;
      }
      else if (ball.position.v1 >= COURT_Y + COURT_HEIGHT - BALL_DIAMETER){
        ball.position.v1 = COURT_Y + COURT_HEIGHT - BALL_DIAMETER;
        ball.velocity.v1 = -ball.velocity.v1;
      }
      else if (ball.position.v0 < COURT_X) {
        score_point(&player_2);
      }
      else if (ball.position.v0 > COURT_X + COURT_WIDTH - BALL_DIAMETER) {
        score_point(&player_1);
      }
    }
  }
}

void pong_move_player(uint32_t player_id, pong_player_direction_t direction){
  switch(state) {
    case STATE_RUNNING:
      if (player_id == 1) {
        player_1.velocity.v1 = PLAYER_SPEED * (direction % 2);
      }
      else if (player_id == 2) {
        player_2.velocity.v1  = PLAYER_SPEED * (direction % 2);
      }
      break;
    case STATE_START:
      // Only change state on keypress, ignore releases
      if (direction != PONG_DIRECTION_STOP){
        state = STATE_RUNNING;
      }
      break;
    case STATE_WIN:
      if (direction != PONG_DIRECTION_STOP){
        reset_game();
      }
      break;
  }
}

void draw(void) {
  // Court top edge
  renderer_draw_rect(COURT_X - COURT_EDGE_THICKNESS, COURT_Y - COURT_EDGE_THICKNESS, COURT_WIDTH + 2 * COURT_EDGE_THICKNESS, COURT_EDGE_THICKNESS);
  // Court bottom edge
  renderer_draw_rect(COURT_X - COURT_EDGE_THICKNESS, COURT_Y + COURT_HEIGHT, COURT_WIDTH + 2 * COURT_EDGE_THICKNESS, COURT_EDGE_THICKNESS);
  // Court left side
  renderer_draw_rect(COURT_X - COURT_EDGE_THICKNESS, COURT_Y - COURT_EDGE_THICKNESS, COURT_EDGE_THICKNESS, COURT_HEIGHT + 2 * COURT_EDGE_THICKNESS);
  // Court right side
  renderer_draw_rect(COURT_X + COURT_WIDTH, COURT_Y - COURT_EDGE_THICKNESS, COURT_EDGE_THICKNESS, COURT_HEIGHT + 2 * COURT_EDGE_THICKNESS);

  sprintf(score_text, "%d", player_1.score);
  renderer_draw_string(COURT_X + COURT_WIDTH / 4, COURT_Y / 2, 2, score_text, strlen(score_text), JUSTIFY_CENTRE);
  sprintf(score_text, "%d", player_2.score);
  renderer_draw_string(COURT_X + 3 * COURT_WIDTH / 4, COURT_Y / 2, 2, score_text, strlen(score_text), JUSTIFY_CENTRE);

  switch(state) {
    case STATE_RUNNING:
      // Net
      for (int i = 0; i < NET_SEGMENTS; i+=2) {
        renderer_draw_rect(COURT_X + ((COURT_WIDTH - NET_THICKNESS) / 2), (COURT_Y - COURT_EDGE_THICKNESS/2) + i * NET_SEGMENT_LENGTH, NET_THICKNESS, NET_SEGMENT_LENGTH);
      }

      renderer_draw_rect(player_1.position.v0, player_1.position.v1, player_1.width, player_1.height);
      renderer_draw_rect(player_2.position.v0, player_2.position.v1, player_2.width, player_2.height);
      renderer_draw_rect(ball.position.v0, ball.position.v1, ball.diameter, ball.diameter);
      break;
    
    case STATE_WIN:
      if(player_1.score > player_2.score) {
        renderer_draw_string(COURT_X + COURT_WIDTH / 2, COURT_Y + COURT_HEIGHT / 4, 3, "Player 1 wins!", 14, JUSTIFY_CENTRE);
      } else {
        renderer_draw_string(COURT_X + COURT_WIDTH / 2, COURT_Y + COURT_HEIGHT / 4, 3, "Player 2 wins!", 14, JUSTIFY_CENTRE);
      }
      renderer_draw_string(COURT_X + COURT_WIDTH / 2, COURT_Y + COURT_HEIGHT / 2, 2, "Press button to restart", 23, JUSTIFY_CENTRE);
      break;

    case STATE_START:
      renderer_draw_string(COURT_X + COURT_WIDTH / 2, COURT_Y + COURT_HEIGHT / 4, 3, "Pi Pico Pong", 12, JUSTIFY_CENTRE);
      renderer_draw_string(COURT_X + COURT_WIDTH / 2, COURT_Y + COURT_HEIGHT / 2, 2, "By Alan Reed", 12, JUSTIFY_CENTRE);
      renderer_draw_string(COURT_X + COURT_WIDTH / 2, COURT_Y + 3 * COURT_HEIGHT / 4, 2, "Press button to start", 21, JUSTIFY_CENTRE);
      break;
  }
}

static void reset_game(void) {
  player_1 = (player_t){.id = 1,
                        .position = (vec2_t){.v0 = PLAYER_1_START_X, .v1 = PLAYER_START_Y},
                        .velocity = (vec2_t){.v0 = 0, .v1 = 0}, 
                        .width = PLAYER_WIDTH,
                        .height = PLAYER_HEIGHT,
                        .score = 0};
  player_2 = (player_t){.id = 2,
                        .position = (vec2_t){.v0 = PLAYER_2_START_X, .v1 = PLAYER_START_Y},
                        .velocity = (vec2_t){.v0 = 0, .v1 = 0},
                        .width = PLAYER_WIDTH,
                        .height = PLAYER_HEIGHT,
                        .score = 0};

  reset_ball(1 + (rand() < RAND_MAX / 2));
  update_game_flag = false;
  state = STATE_START;
}

static void reset_ball(int player_id) {
  ball = (ball_t){.position = (vec2_t){.v0 = BALL_START_X, .v1 = BALL_START_Y}, .diameter = BALL_DIAMETER};
  double angle = BALL_START_ANGLE_MIN + ((double)rand() / (double)RAND_MAX) * (BALL_START_ANGLE_MAX - BALL_START_ANGLE_MIN);
  if (rand() > RAND_MAX / 2) {
    angle *= -1;
  }
  // Start the ball towards the player that scored
  if (player_id == 1) {
    ball.velocity.v0 = -BALL_START_SPEED * cos(angle);
  }
  else {
    ball.velocity.v0  = BALL_START_SPEED * cos(angle);
  }
  ball.velocity.v1 = BALL_START_SPEED * sin(angle);
}

static void bounce_ball(float surface_angle) {
  vec2_t v = ball.velocity;
  vec2_t n = (vec2_t){cos(surface_angle), sin(surface_angle)};

  vec2_t u = vec2_scale(n, vec2_dot(v, n));
  vec2_t w = vec2_subtract(v, u);
  vec2_t v_after = vec2_subtract(w, u);

  vec2_t v_increase = vec2_scale(v_after, BALL_ACCELERATION / vec2_length(v_after));
  ball.velocity = vec2_add(v_after, v_increase);
}

static void update_player_position(player_t *player){
  player->position = vec2_add(player->position, player->velocity);
  if (player->position.v1 < PLAYER_Y_MIN) {
    player->position.v1 = PLAYER_Y_MIN;
  }
  else if (player->position.v1 > PLAYER_Y_MAX) {
    player->position.v1 = PLAYER_Y_MAX;
  }
}

static void score_point(player_t *player) {
  reset_ball(player->id);
  player->score++;
  if (player->score >= PLAYER_WIN_THRESHOLD) {
    state = STATE_WIN;
  }
}
