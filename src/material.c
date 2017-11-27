#include "material.h"

#include "gl.h"

/*
 * Apply a material for GL to use for lighting up your mesh
 */
void applyMaterial(Material* material)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat*) &material->ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat*) &material->diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat*) &material->specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material->shininess);
}
