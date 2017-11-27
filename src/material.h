#pragma once

#include "vec.h"

/*
 * All of the stuff you need to specify a material for your meshes
 */
typedef struct {
	Vec4f ambient, diffuse, specular;
	float shininess;
} Material;

void applyMaterial(Material* material);
