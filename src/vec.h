#pragma once

// 2D, 3D and 4D vector structs, only 2D and 3D have operations defined for them as that's all we need for now
typedef struct {
	float x, y;
} Vec2f;

typedef struct {
	float x, y, z;
} Vec3f;

typedef struct {
	float x, y, z, w;
} Vec4f;

Vec2f addVec2f(Vec2f a, Vec2f b);
Vec2f subVec2f(Vec2f a, Vec2f b);
Vec2f mulVec2f(Vec2f v, float s);
Vec2f divVec2f(Vec2f v, float s);
Vec2f negVec2f(Vec2f v);

float dotVec2f(Vec2f a, Vec2f b);
float magVec2f(Vec2f v);
Vec2f normaliseVec2f(Vec2f v);

Vec3f addVec3f(Vec3f a, Vec3f b);
Vec3f subVec3f(Vec3f a, Vec3f b);
Vec3f mulVec3f(Vec3f v, float s);
Vec3f divVec3f(Vec3f v, float s);
Vec3f negVec3f(Vec3f v);

float dotVec3f(Vec3f a, Vec3f b);
float magVec3f(Vec3f v);
Vec3f normaliseVec3f(Vec3f v);
Vec3f crossVec3f(Vec3f a, Vec3f b);
