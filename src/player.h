#pragma once

#include "util.h"
#include "controls.h"
#include "mesh.h"
#include "material.h"
#include "anim.h"

/*
 * Our player has position and velocity, which are set from the speed and rotation parameters
 * Gravity is also stored here just because our player is the only object affected by it and you might want to be able to adjust it on the fly
 */
enum { body, mouth, shoulder, elbow, waist, knee, ankle, n_joints } Joint;

typedef struct {
	Vec3f pos, vel, initPos, initVel;
	float speed, xRot, yRot, size, g;
	bool jump, onLog, prepare, ribbit;
	Mesh* mesh;
	Material material;
	float joints[n_joints];
	Interpolator preItps[n_joints];
	Interpolator jumpItps[n_joints];
	Interpolator ribbitItp;
} Player;

void initPlayer(Player* player);
void destroyPlayer(Player* player);
void updatePlayer(Player* player, float dt, Controls* controls, float elapsedTime);
void renderPlayer(Player* player, DrawingFlags* flags);
