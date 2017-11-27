/*
 * util
 * Some useful definitions and functions for drawing and physics
 */
#include "util.h"
#include "gl.h"
#include <SOIL/SOIL.h>

const Vec3f WHITE = { 1.0, 1.0, 1.0 };
const Vec3f RED = { 1.0, 0.0, 0.0 };
const Vec3f GREEN = { 0.0, 1.0, 0.0 };
const Vec3f BLUE = { 0.0, 0.0, 1.0 };
const Vec3f YELLOW = { 1.0, 1.0, 0.0 };
const Vec3f CYAN = { 0.0, 1.0, 1.0 };
const Vec3f PURPLE = { 1.0, 0.0, 1.0 };
const Vec3f BROWN = { 0.15, 0.02, 0.02 };
const Vec3f GRAY = { 0.5, 0.5, 0.5 };
const Vec3f DARKGRAY = { 0.3, 0.3, 0.3 };
const Vec3f SAND = { 0.58, 0.45, 0.26 };
const Vec3f BLACK = { 0.0, 0.0, 0.0 };

// some handy funcitons for getting random values
float getRand() {
	return (float) rand() / (float) RAND_MAX;
}

float getNRand() {
	return getRand() * 2.0f - 1.0f;
}

float getTRand(float min, float max) {
	return getRand() * (max - min) + min;
}

// draw a set of coloured axes at the origin
void drawAxes() {
	glPushAttrib(GL_CURRENT_BIT);
	glBegin(GL_LINES);

	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);

	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1, 0);

	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);

	glEnd();

	glPopAttrib();
}

// load a texture from file using the SOIL library
unsigned int loadTexture(const char* filename) {
	glPushAttrib(GL_TEXTURE_BIT);
	unsigned int id = SOIL_load_OGL_texture(filename, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
	glPopAttrib();
	return id;
}
