#include "particles.h"
#include "gl.h"

#include <string.h>

/*
 * Update the particles's position and velocity with frametime dt
 */
static void integrateParticles(Particles* particles, float dt) {
	Particle * particle;
	int count = 0;

	for (int i = 0; i < particles->num_particles; i++) {
		particle = &particles->particles[i];
		
		// update velocity with gravity
		particle->vel.y -= particles->g * dt;

		// this gives a pretty reasonable integration of acceleration with each step
		particle->pos.y += particle->vel.y * dt + 0.5f * particles->g * dt * dt;
		particle->pos.x += particle->vel.x * dt;
		particle->pos.z += particle->vel.z * dt;

		if (particle->pos.y < -particles->size) {
			particle->jump = false;
			count++;
		}
	}
	if (count == particles->num_particles) {
		particles->spawn = false;
	}
}

static void resetParticles(Particles * particles, Vec3f playerPos) {
	Particle * particle;

	for (int i = 0; i < particles->num_particles; i++) {
		particle = &particles->particles[i];
		particle->pos = playerPos;
		particle->initPos = playerPos;
		particle->speed = getTRand(1.0, 5.0);
		particle->initVel.x = getTRand(-1.0, 1.0);
		particle->initVel.y = getTRand(0.0, 1.0);
		particle->initVel.z = getTRand(-1.0, 1.0);
		particle->initVel = mulVec3f(particle->initVel, particle->speed);
		particle->vel = particle->initVel;
		particle->jump = true;
	}
}

void initParticles(Particles * particles, DrawingFlags * flags) {
	particles->size = 0.05;
	particles->g = 9.8;
	particles->spawn = false;

	particles->mesh = createSphere(flags->segments, flags->segments);
	particles->material = (Material) { { 0.2, 0.2, 0.2, 0 }, { 1, 0, 0, 0 }, { 1, 1, 1, 0 }, 50 };
	particles->num_particles = 100;
	particles->particles = (Particle*) malloc(sizeof(Particle) * particles->num_particles);
	resetParticles(particles, (Vec3f) {0.0, -particles->size, 0.0});
	for (int i = 0; i < particles->num_particles; i++) {
		particles->particles[i].jump = false;
	}
}

/*
 * Cleanup any memory used by the particles
 */
void destroyParticles(Particles* particles) {
	free(particles->particles);
	destroyMesh(particles->mesh);
}

/*
 * Update the particles's state for frametime dt.
 */
void updateParticles(Particles* particles, bool isCollided, Vec3f playerPos, float dt) {
	if (isCollided) {
		particles->spawn = true;
		resetParticles(particles, playerPos);
	}
	if (particles->spawn) {
		integrateParticles(particles, dt);
	}
}

/*
 * Draw the particles's mesh, as well as a parabola showing our jump arc and a visualization of our current velocity
 */
void renderParticles(Particles* particles, DrawingFlags* flags) {
	Particle * particle;
	glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT);

	// // draw the parabola from the starting point of the jump
	// for (int i = 0; i < particles->num_particles; i++) {
	// 	particle = &particles->particles[i];
	// 	glPushMatrix();
	// 	glTranslatef(particle->initPos.x, particle->initPos.y, particle->initPos.z);
	// 	drawParabola(BLUE, particle->initVel, particles->g, flags);
	// 	glPopMatrix();
	// }

	for (int i = 0; i < particles->num_particles; i++) {
		particle = &particles->particles[i];
		glPushMatrix();
			glTranslatef(particle->pos.x, particle->pos.y, particle->pos.z);

			// draw the particles mesh at our current position
			glPushMatrix();
				glScalef(particles->size, particles->size, particles->size);
				applyMaterial(&particles->material);
				submitColor(RED);
				if (particle->jump) {
					renderMesh(particles->mesh, flags);
				}
			glPopMatrix();
			
			// // draw the visualization of the particles's velocity at particle current position
			// glBegin(GL_LINES);
			// drawLine(PURPLE, (Vec3f) { 0, 0, 0 }, mulVec3f(particle->vel, 0.1)); 
			// glEnd();
		glPopMatrix();
	}

	glPopAttrib();
}
