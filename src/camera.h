#pragma once

#include "vec.h"

typedef struct {
	float xRot, yRot, zoom;
	Vec3f pos;
	int lastX, lastY;
	int width, height;
	float fovY, near, far;
} Camera;

void initCamera(Camera* camera);
void applyViewMatrix(Camera* camera);
void applyProjectionMatrix(Camera* camera);
