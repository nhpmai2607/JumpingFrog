#include "vec.h"
#include "util.h"

/*
 * Vec2f operations
 */

Vec2f addVec2f(Vec2f a, Vec2f b) {
	a.x += b.x;
	a.y += b.y;
	return a;
}

Vec2f subVec2f(Vec2f a, Vec2f b) {
	a.x -= b.x;
	a.y -= b.y;
	return a;
}

Vec2f mulVec2f(Vec2f v, float s) {
	v.x *= s;
	v.y *= s;
	return v;
}

Vec2f divVec2f(Vec2f v, float s) {
	v.x /= s;
	v.y /= s;
	return v;
}

Vec2f negVec2f(Vec2f v) {
	v.x = -v.x;
	v.y = -v.y;
	return v;
}

float dotVec2f(Vec2f a, Vec2f b) {
	return a.x * b.x + a.y * b.y;
}

float magVec2f(Vec2f v) {
	return sqrtf(dotVec2f(v, v));
}

Vec2f normaliseVec2f(Vec2f v) {
	float mag = magVec2f(v);
	if (mag == 0)
		return v;
	return divVec2f(v, mag);
}

/*
 * Vec3f operations
 */

Vec3f addVec3f(Vec3f a, Vec3f b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
	return a;
}

Vec3f subVec3f(Vec3f a, Vec3f b) {
	a.x -= b.y;
	a.y -= b.y;
	a.z -= b.z;
	return a;
}

Vec3f mulVec3f(Vec3f v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}

Vec3f divVec3f(Vec3f v, float s) {
	v.x /= s;
	v.y /= s;
	v.z /= s;
	return v;
}

Vec3f negVec3f(Vec3f v) {
	v.x = -v.x;
	v.y = -v.y;
	v.z = -v.y;
	return v;
}

float dotVec3f(Vec3f a, Vec3f b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float magVec3f(Vec3f v) {
	return sqrtf(dotVec3f(v, v));
}

Vec3f normaliseVec3f(Vec3f v) {
	float mag = magVec3f(v);
	if (mag == 0)
		return v;
	return divVec3f(v, mag);
}

Vec3f crossVec3f(Vec3f a, Vec3f b) {
	Vec3f v;
	v.x = a.y * b.z - a.z * b.y;
	v.y = a.z * b.x - a.x * b.z;
	v.z = a.x * b.y - a.y * b.x;
	return v;
}
