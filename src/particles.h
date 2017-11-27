#pragma once

#include "util.h"
#include "mesh.h"
#include "material.h"

typedef struct {
	Vec3f pos, vel, initPos, initVel;
	float speed;
	bool jump;
} Particle;

typedef struct {
	float size, g;
	Mesh* mesh;
	Material material;
	Particle * particles;
	bool spawn;
	int num_particles;
} Particles;

void initParticles(Particles* particles, DrawingFlags* flags);
void destroyParticles(Particles* particles);
void updateParticles(Particles* particles, bool isCollided, Vec3f playerPos, float dt);
void renderParticles(Particles* particles, DrawingFlags* flags);
