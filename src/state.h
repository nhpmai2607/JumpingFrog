#pragma once

#include "player.h"
#include "level.h"
#include "controls.h"
#include "mesh.h"
#include "camera.h"
#include "skybox.h"
#include "particles.h"

/*
 * All of the global state for our main functions is declared here
 */
typedef struct {
	Player player;
	Level level;
	Controls controls;
	Camera camera;
	DrawingFlags drawingFlags;
	int score, lives;
	bool halt;
	int frames;
	float frameRate, frameRateInterval, lastFrameRateT;
	Skybox skybox;
	Particles particles;
} Globals;
