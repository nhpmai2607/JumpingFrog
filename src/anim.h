#pragma once

#include "util.h"

/* Animation variables. */
typedef struct {
	float time;
	float value;
} KeyFrame;

typedef struct {
  int nKeyFrames;
  KeyFrame keyFrames[10];
  float startTime;
} Interpolator;

float lerp(float t0, float v0, float t1, float v1, float t);

int findInterval(KeyFrame kf[], int n, float t);

/*
 * true : still animating
 * false: stop animating
 */
bool animate(float elapsedTime, Interpolator interpolator, float * rot);