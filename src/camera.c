#include "camera.h"
#include "gl.h"

/*
 * Initialise the parameters of our camera
 */
void initCamera(Camera* camera) {
	camera->zoom = 2.0;
	camera->xRot = 0;
	camera->yRot = 45;
	camera->fovY = 75;
	camera->near = 0.01;
	camera->far = 1000;
}

/*
 * Apply our view transformation based on the camera's zoom, rotation and position
 */
void applyViewMatrix(Camera* camera) {
	glLoadIdentity();
	glTranslatef(0, 0, -camera->zoom);
	glRotatef(camera->yRot, 1, 0, 0);
	glRotatef(camera->xRot, 0, 1, 0);
	glTranslatef(-camera->pos.x, -camera->pos.y, -camera->pos.z);
}

/*
 * Create a projection matrix based on the camera's fov, dimensions and near and far planes
 */
void applyProjectionMatrix(Camera* camera) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(camera->fovY, (float)camera->width / (float)camera->height, camera->near, camera->far);
	glMatrixMode(GL_MODELVIEW);
}
