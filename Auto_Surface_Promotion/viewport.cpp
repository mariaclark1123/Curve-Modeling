#include "viewport.h"
using namespace std;
#ifdef DEBUG
void PRINT_CTRLPTS(CubicBezierCurve* crv) {
	int i;
	printf("curve %p\n[\n", crv);
	for (i = 0; i<4; ++i)
		printf("[%f, %f]\n", crv->control_pts[i][X], crv->control_pts[i][Y]);
	printf("]\n");
}
#endif
Vec3d rotate(Vec3d input, Vec3d rotateVector, float angle) {
	Matrix3d matrix;
	Vec3d rotated = input;
	matrix.set(angle, rotateVector);
	matrix.transform(rotated);
	return rotated;
}
Vec3d unProjectToEye(Vec3d vector, Vec3d& eye, Vec3d& center, Vec3d& upVector) {
	Vec3d zAxis;
	zAxis.sub(center, eye);
	zAxis.normalize();
	Vec3d xAxis;
	xAxis.cross(upVector, zAxis);
	xAxis.normalize();
	Vec3d yAxis;
	yAxis.cross(zAxis, xAxis);
	yAxis.normalize();
	Vec3d newVector;
	xAxis.scale(vector.x);
	newVector.add(xAxis);
	yAxis.scale(vector.y);
	newVector.add(yAxis);
	zAxis.scale(vector.z);
	newVector.add(zAxis);
	return newVector;
}
Vec3d getMousePoint(int mouseX, int mouseY, int width, int height, float radius) {
	float x = mouseX - width / 2.0;
	float y = mouseY - height / 2.0;
	float zs = radius*radius - x*x - y*y;
	if (zs <= 0) {
		zs = 0;
	}
	float z = std::sqrt(zs);
	return Vec3d(x, y, z);
}