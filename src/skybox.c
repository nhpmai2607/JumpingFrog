#include "skybox.h"
#include "gl.h"

#include <string.h>

static void loadSkyboxTexture(Skybox * skybox) {
	const unsigned int texture[n_skybox_textures] = 
					{
						loadTexture("res/skybox/negz.jpg"), //negZ
						loadTexture("res/skybox/posz.jpg"), //posZ
						loadTexture("res/skybox/negx.jpg"), //negX
						loadTexture("res/skybox/posx.jpg"),	//posX
						loadTexture("res/skybox/posy.jpg"), //posY
						loadTexture("res/skybox/negy.jpg")	//negY
					};
	memcpy(skybox->texture, texture, sizeof(texture));
}

void initSkybox(Skybox * skybox) {
	skybox->mesh = createCube();
	skybox->material = (Material) { { 0.2, 0.2, 0.2, 0 }, { 0.0, 0.5, 1.0, 0 }, { 1, 1, 1, 0 }, 50 };
	loadSkyboxTexture(skybox);
}

void renderSkybox(Skybox * skybox, DrawingFlags* flags) {
	Mesh * mesh = skybox->mesh;
	glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT);
	
	if (flags->textures)
		glEnable(GL_TEXTURE_2D);

	glDisable(GL_DEPTH_TEST);

	for (int i = 0; i < (int) mesh->numVerts; i++) {
		if (i % 4 == 0) {
			glBindTexture(GL_TEXTURE_2D, skybox->texture[i / 4]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glBegin(GL_QUADS);
		}
		applyMaterial(&skybox->material);
		submitColor(BLUE);
		Vec3f vertex = mesh->verts[i].pos;
		Vec2f tex = mesh->verts[i].tc;
		glTexCoord2f(tex.x, tex.y);
		glVertex3f(vertex.x, vertex.y, vertex.z);
		if (i % 4 == 3) { glEnd(); }
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	
	glEnable(GL_DEPTH_TEST);
	glPopAttrib();
}

void destroySkybox(Skybox * skybox) {
	destroyMesh(skybox->mesh);
}