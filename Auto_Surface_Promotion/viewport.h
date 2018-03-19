#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <vector>
#include <iostream>
#ifndef EPS
#define EPS         1.110223024E-16        /* data type is float */
#endif
#define LARGE    FLT_MAX
//enum XY { X = 0, Y };
enum TYPE { AABB = 0, OBB };
struct Vec3d {
	float x;
	float y;
	float z;

	Vec3d() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}
	Vec3d(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	void set(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	void sub(Vec3d t1, Vec3d t2)
	{
		this->x = t1.x - t2.x;
		this->y = t1.y - t2.y;
		this->z = t1.z - t2.z;
	}
	void cross(Vec3d v1, Vec3d v2)
	{
		float x, y;
		x = v1.y*v2.z - v1.z*v2.y;
		y = v2.x*v1.z - v2.z*v1.x;
		this->z = v1.x*v2.y - v1.y*v2.x;
		this->x = x;
		this->y = y;
	}
	void scale(float s)
	{
		this->x *= s;
		this->y *= s;
		this->z *= s;
	}
	void add(Vec3d t1)
	{
		this->x = this->x + t1.x;
		this->y = this->y + t1.y;
		this->z = this->z + t1.z;
	}
	void normalize()
	{
		float norm;
		norm = 1.0f / std::sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
		this->x *= norm;
		this->y *= norm;
		this->z *= norm;
	}
	float length()
	{
		return std::sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
	}
	float dot(Vec3d v1)
	{
		return (this->x*v1.x + this->y*v1.y + this->z*v1.z);
	}
	float angle(Vec3d v1)
	{
		float vDot = this->dot(v1) / (this->length()*v1.length());
		if (vDot < -1.0) vDot = -1.0;
		if (vDot >  1.0) vDot = 1.0;
		return((float)(std::acos(vDot)));
	}
	float distance(Vec3d p1)
	{
		float dx, dy, dz;
		dx = this->x - p1.x;
		dy = this->y - p1.y;
		dz = this->z - p1.z;
		return std::sqrt(dx*dx + dy*dy + dz*dz);
	}
	float* genArray() {
		float* v = new float[3];
		v[0] = x;
		v[1] = y;
		v[2] = z;
		return v;
	}
};
struct Matrix3d {
	float m00, m01, m02, m10, m11, m12, m20, m21, m22;
	Matrix3d() {
	}
	void set(float angle, Vec3d v) {
		set(angle, v.x, v.y, v.z);
	}
	void set(float angle, float x, float y, float z) {
		float mag = std::sqrt(x*x + y*y + z*z);
		if (mag < EPS) {
			m00 = 1.0;
			m01 = 0.0;
			m02 = 0.0;
			m10 = 0.0;
			m11 = 1.0;
			m12 = 0.0;
			m20 = 0.0;
			m21 = 0.0;
			m22 = 1.0;
		}
		else {
			mag = 1.0f / mag;
			float ax = x*mag;
			float ay = y*mag;
			float az = z*mag;
			float sinTheta = std::sin(angle);
			float cosTheta = std::cos(angle);
			float t = 1.0f - cosTheta;
			float xz = ax * az;
			float xy = ax * ay;
			float yz = ay * az;
			m00 = t * ax * ax + cosTheta;
			m01 = t * xy - sinTheta * az;
			m02 = t * xz + sinTheta * ay;
			m10 = t * xy + sinTheta * az;
			m11 = t * ay * ay + cosTheta;
			m12 = t * yz - sinTheta * ax;
			m20 = t * xz - sinTheta * ay;
			m21 = t * yz + sinTheta * ax;
			m22 = t * az * az + cosTheta;
		}
	}
	void transform(Vec3d& t) {
		float x, y, z;
		x = m00* t.x + m01*t.y + m02*t.z;
		y = m10* t.x + m11*t.y + m12*t.z;
		z = m20* t.x + m21*t.y + m22*t.z;
		t.set(x, y, z);
	}
};

Vec3d rotate(Vec3d input, Vec3d rotateVec3dtor, float angle);
Vec3d unProjectToEye(Vec3d Vec3dtor, Vec3d& eye, Vec3d& center, Vec3d& upVec3dtor);
Vec3d getMousePoint(int mouseX, int mouseY, int width, int height, float radius);
#endif /* _VIEWPORT_H_ */