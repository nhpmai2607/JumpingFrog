#pragma once

#include "util.h"
#include "mesh.h"
#include "material.h"
#include "camera.h"

enum { SKYNZ, SKYPZ, SKYNX, SKYPX, SKYPY, SKYNY, n_skybox_textures } SkyboxTexture;
	
typedef struct {
	Mesh* mesh;
	Material material;
	unsigned int texture[n_skybox_textures];
} Skybox;

void initSkybox(Skybox * skybox);
void renderSkybox(Skybox * skybox, DrawingFlags* flags);
void destroySkybox(Skybox * skybox);