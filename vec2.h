#ifndef __VEC2_H__
#define __VEC2_H__

#include <stdio.h>
#include <math.h>

typedef struct {
  float v0;
  float v1;
}
vec2_t;

inline vec2_t vec2_add(vec2_t p1, vec2_t p2) {
  return (vec2_t){.v0=p1.v0 + p2.v0, .v1 = p1.v1 + p2.v1};
}

inline vec2_t vec2_subtract(vec2_t p1, vec2_t p2) {
  return (vec2_t){.v0=p1.v0 - p2.v0, .v1 = p1.v1 - p2.v1};
}

inline float vec2_dot(vec2_t p1, vec2_t p2) {
  return (p1.v0 * p2.v0) + (p1.v1 * p2.v1);
}

inline vec2_t vec2_scale(vec2_t p, float scalar) {
  return (vec2_t){.v0 = p.v0 * scalar, .v1 = p.v1 * scalar};
}

inline float vec2_length(vec2_t vec) {
  return sqrt((vec.v0 * vec.v0) + (vec.v1 * vec.v1));
}

inline void vec2_print(vec2_t vec) {
  printf("v: (%f, %f)\r\n", vec.v0, vec.v1);
  printf("len %f\r\n", vec2_length(vec));
}

#endif