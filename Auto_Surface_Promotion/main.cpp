#pragma warning(disable:4146)
#pragma warning(disable:4996)
#define STB_IMAGE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "curve.h"
#include "string.h"
#include "stb_image.h"
#include <gl/freeglut.h>

/* global */
Model TMJ, Fossa, Tubercle;
GLsizei width = 800, height = 800;
float viewportwidth = 400, viewportheight = 300;

int selectedscene = 0;
int selected = -1;
/*3D viewport*/
Vec3d eye;
Vec3d center;
Vec3d upVector;
bool isDragging = false;
bool show_TMJ = true;
bool show_Fossa = false;
bool show_Tubercle = false;
bool show1 = true, show2 = true;
bool sur_out = false;  //output surface to file
bool FT_changed = false;
REAL radius;
REAL FT_inter = 0.01;

int mouseButton = -1;
int lastX = -1;
int lastY = -1;
int slice_index = 0;

void init()
{
	//Load texture
	unsigned char *data = stbi_load("bone7.jpg", &width, &height, 0, 0);
	if (data)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(data);

	/*Get vertex, face infomation*/
	TMJ.InitModel("Model/Extended1_left_TMJ.obj", false);
	Fossa.InitModel("Model/Extended1_left_Fossa.obj", true);
	Tubercle.InitModel("Model/Extended1_left_Fossa.obj", true);

	/*Set Fossa_right information*/
	Fossa.Divide_SetSlice(true, &FT_inter);
	Fossa.SetSurface();

	/*Set Tubercle_right information*/
	Tubercle.Divide_SetSlice(false, &FT_inter);
	Tubercle.SetSurface();

	/*Set TMJ information*/
	TMJ.SetSlice();
	TMJ.SetSurface();

	eye = Vec3d(60, 60, 60);

	center = Vec3d(0, 0, 0);
	upVector = Vec3d(0, 1, 0);
}

//int hit_index(int x, int y, int scene)
//{
//	int xx, yy;
//	switch (scene)
//	{
//	case 1:
//		xx = 0, yy = 1;
//		break;
//	case 3:
//		xx = 0, yy = 2;
//		break;
//	case 4:
//		xx = 1, yy = 2;
//		break;
//	}
//	int min = 30;
//	int minp = -1;
//	/*for (int i = 0; i < 4; i++)
//	{
//		for (int j = 0; j < 4; j++)
//		{
//			float tx = bezsur.control_pts[i][j][xx] - x;
//			float ty = bezsur.control_pts[i][j][yy] - y;
//			if ((tx * tx + ty * ty) < min)
//			{
//				min = (tx * tx + ty * ty);
//				minp = i * 10 + j;
//			}
//		}
//	}*/
//	return minp;
//}

void reshape_callback(GLint nw, GLint nh)
{
	width = nw;
	height = nh;

	viewportwidth = width / 2.0f;
	viewportheight = height / 2.0f;
	radius = std::sqrt(viewportwidth * viewportwidth + viewportheight * viewportheight) / 2;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glutPostRedisplay();
}

void display_callback(void)
{
	/*Set Fossa information*/
	if (FT_changed)
	{
		Fossa.Divide_SetSlice(true, &FT_inter);
		Fossa.SetSurface();

		/*Set Tubercle information*/
		Tubercle.Divide_SetSlice(false, &FT_inter);
		Tubercle.SetSurface();
		FT_changed = false;
	}
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3f(0, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(-1, 0, 0);
	glVertex3f(1, 0, 0);
	glEnd();
	glBegin(GL_LINES);
	glVertex3f(0, -1, 0);
	glVertex3f(0, 1, 0);
	glEnd();

	glPointSize(8.0f);

	/*-----------2D Slice----------*/
	glViewport(0, viewportheight, viewportwidth, viewportheight);
	glLoadIdentity();
	gluOrtho2D(-15, 15, -15, 15);
	//annotation
	char str[4];
	_itoa_s(slice_index, str, 10);

	glColor3ub(0, 0, 0);
	glRasterPos2f(-14.5, 14);
	if (show_Fossa)
		drawString("Fosa Slice");
	else if (show_TMJ)
		drawString("TMJ Slice");
	drawString(str);

	glBegin(GL_LINES);
	glColor3f(0, 1.0f, 0);
	glVertex2f(0, 0);
	glVertex2f(500.0f, 0);
	glColor3f(0, 0, 1.0f);
	glVertex2f(0, 0);
	glVertex2f(0, 500.0f);
	glEnd();

	/*Draw model slice*/
	for (int j = 0; j < 2; j++)
	{
		if (show_TMJ)
			TMJ.all_slice[slice_index].cur_maxz[j].DrawCurve2D();
		else
		{
			Fossa.all_slice[slice_index].cur_minz[j].DrawCurve2D();
			Tubercle.all_slice[slice_index].cur_minz[j].DrawCurve2D();
		}
	}
	glColor3ub(255, 0, 0);
	glPointSize(9.0);
	glBegin(GL_POINTS);
	//Draw red points
	for (int i = 0; i < MV_num; i++)
	{
		if (show_TMJ)
			glVertex2f(TMJ.all_slice[slice_index].maxz_vertex[i][Y], TMJ.all_slice[slice_index].maxz_vertex[i][Z]);
		else
		{
			glVertex2f(Fossa.all_slice[slice_index].minz_vertex[i][Y], Fossa.all_slice[slice_index].minz_vertex[i][Z]);
			glVertex2f(Tubercle.all_slice[slice_index].minz_vertex[i][Y], Tubercle.all_slice[slice_index].minz_vertex[i][Z]);
		}
	}
	glEnd();

	/*-------------TMJ + Fossa-----------*/
	glViewport(viewportwidth, 0, viewportwidth, viewportheight);
	glLoadIdentity();
	gluOrtho2D(0, (double)viewportwidth, 0, (double)viewportheight);

	//annotation
	glColor3ub(0, 0, 0);
	glRasterPos2f(10.0f, viewportheight - 20);
	drawString("TMJ + Fossa + Tubercle");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(25, width / (double)height, 0.1, 25000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, upVector.x, upVector.y, upVector.z);

	glEnable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(500.0f, 0, 0);
	glColor3f(0, 1.0f, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 500.0f, 0);
	glColor3f(0, 0, 1.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 500.0f);
	glEnd();

	if (show1)
	{
		TMJ.DrawModel();
		TMJ.DrawSliceSurface(2);
	}
	if (show2)
	{
		Fossa.DrawModel();
		Fossa.DrawSliceSurface(3);
		Tubercle.DrawSliceSurface(3);
	}

	glDisable(GL_DEPTH_TEST);

	/*------------Fossa----------*/
	glViewport(0, 0, viewportwidth, viewportheight);
	glLoadIdentity();
	gluOrtho2D(0, (double)viewportwidth, 0, (double)viewportheight);

	//annotation
	glColor3ub(0, 0, 0);
	glRasterPos2f(10.0f, viewportheight - 20);
	drawString("Fossa");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(25, width / (double)height, 0.1, 25000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, upVector.x, upVector.y, upVector.z);

	glEnable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(500.0f, 0, 0);
	glColor3f(0, 1.0f, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 500.0f, 0);
	glColor3f(0, 0, 1.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 500.0f);
	glEnd();

	if (show2)
	{
		Fossa.DrawModel();
		Tubercle.DrawModel();
	}
	/*draw slice's 4 cur*/
	Fossa.DrawSliceCurve(3, slice_index);
	Fossa.DrawSliceSurface(3);

	Tubercle.DrawSliceCurve(3, slice_index);
	Tubercle.DrawSliceSurface(3);

	glDisable(GL_DEPTH_TEST);

	/*-----------TMJ----------*/
	glViewport(viewportwidth, viewportheight, viewportwidth, viewportheight);
	glLoadIdentity();
	gluOrtho2D(0, (double)viewportwidth, 0, (double)viewportheight);

	//annotation
	glColor3ub(0, 0, 0);
	glRasterPos2f(10.0f, viewportheight - 20);
	drawString("TMJ");

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(25, width / (double)height, 0.1, 25000);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, upVector.x, upVector.y, upVector.z);

	glEnable(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	glColor3f(1.0f, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(500.0f, 0, 0);
	glColor3f(0, 1.0f, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 500.0f, 0);
	glColor3f(0, 0, 1.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 500.0f);
	glEnd();

	if (show1)
		TMJ.DrawModel();
	/*draw slice's 4 cur*/
	TMJ.DrawSliceCurve(2, slice_index);
	TMJ.DrawSliceSurface(2);

	glDisable(GL_DEPTH_TEST);

	if (sur_out)
	{
		cout << "output file success" << endl;
		sur_out = false;
		TMJ.OutFile("TMJHead_quad.qda", 0);
		Fossa.OutFile("Fossa_quad.qda", 1);
		Tubercle.OutFile("Tubercle_quad.qda", 1);
	}
	glutSwapBuffers();
}

void mouse_callback(GLint button, GLint action, GLint x, GLint y)
{
	int scene = 0;
	if (x < viewportwidth)
	{
		if (y < viewportheight)
			scene = 1;
		else
		{
			scene = 3;
			y -= (int)viewportheight;
		}
	}
	else
	{
		x -= (int)viewportwidth;
		if (y < viewportheight)
			scene = 2;
		else
		{
			scene = 4;
			y -= (int)viewportheight;
		}
	}

	if (action == GLUT_UP)
	{
		isDragging = false;
		mouseButton = -1;
	}

	if (scene == 2)
	{
		if (action == GLUT_DOWN)
		{
			mouseButton = button;
			isDragging = true;
			lastX = x;
			lastY = y;
		}
	}
	/*else
	{
	if (button == GLUT_LEFT_BUTTON)
	{
	switch (action)
	{
	case GLUT_DOWN:
	selectedscene = scene;
	selected = hit_index(x, (int)viewportheight - y, scene);
	break;
	case GLUT_UP:
	selected = -1;
	break;
	default: break;
	}
	}
	}*/
	glutPostRedisplay();
}

void mouse_move_callback(GLint x, GLint y)
{
	Vec3d lastP = getMousePoint(lastX, lastY, viewportwidth, viewportheight, radius);
	Vec3d currentP = getMousePoint(x - viewportwidth, y, viewportwidth, viewportheight, radius);

	if (mouseButton == GLUT_LEFT_BUTTON)
	{
		Vec3d rotateVector;
		rotateVector.cross(currentP, lastP);
		double angle = -currentP.angle(lastP) * 2;
		rotateVector = unProjectToEye(rotateVector, eye, center, upVector);

		Vec3d dEye;
		dEye.sub(center, eye);
		dEye = rotate(dEye, rotateVector, -angle);
		upVector = rotate(upVector, rotateVector, -angle);
		eye.sub(center, dEye);
	}
	else if (mouseButton == GLUT_RIGHT_BUTTON) {
		Vec3d dEye;
		dEye.sub(center, eye);
		double offset = 0.025;
		if ((y - lastY) < 0) {
			dEye.scale(1 - offset);
		}
		else {
			dEye.scale(1 + offset);
		}
		eye.sub(center, dEye);
	}
	else if (mouseButton == GLUT_MIDDLE_BUTTON) {
		double dx = x - viewportwidth - lastX;
		double dy = y - lastY;
		if (dx != 0 || dy != 0)
		{
			Vec3d moveVector(dx, dy, 0);
			moveVector = unProjectToEye(moveVector, eye, center, upVector);
			moveVector.normalize();
			double eyeDistance = Vec3d(eye).distance(Vec3d(center));
			moveVector.scale(std::sqrt(dx*dx + dy*dy) / 1000 * eyeDistance);
			center.add(moveVector);
			eye.add(moveVector);
		}
	}

	lastX = x - viewportwidth;
	lastY = y;

	if (selected != -1)
	{
		int xx = 0;
		int yy = 0;
		switch (selectedscene)
		{
		case 1:
			xx = 0, yy = 1;
			break;
		case 3:
			xx = 0, yy = 2;
			y -= (int)viewportheight;
			break;
		case 4:
			xx = 1, yy = 2;
			x -= (int)viewportwidth;
			y -= (int)viewportheight;
			break;
		}
		x = MAX(x, 0);
		x = MIN(x, (int)viewportwidth);
		y = MAX((int)viewportheight - y, 0);
		y = MIN(y, (int)viewportheight);
		//		bezsur.control_pts[selected / 10][selected % 10][xx] = static_cast<float>(x);
		//	bezsur.control_pts[selected / 10][selected % 10][yy] = static_cast<float>(y);
		//	bezsur.CalculateSurface3D();
	}
	glutPostRedisplay();
}

// void glutKeyboardFunc(void (*func)(unsigned char key, int x, int y));
void keyboard_callback(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '1':
		show1 = (show1 + 1) % 2;
		break;
	case '2':
		show2 = (show2 + 1) % 2;
		break;
	case 'i': case 'I':
		init();
		break;
	case 'f': case 'F':
		show_Fossa = true;
		show_TMJ = false;
		break;

	case 't': case 'T':
		show_TMJ = true;
		show_Fossa = false;
		break;

	case 'o': case 'O':
		sur_out = true;
		break;
	case (27): exit(0); break;

	default: break;
	}
	glutPostRedisplay();
}

void specialkey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		slice_index = (slice_index - 1 + 13) % 13;
		break;
	case GLUT_KEY_RIGHT:
		slice_index = (slice_index + 1) % 13;
		break;
	case GLUT_KEY_UP:
		FT_inter = FT_inter + 0.1;
		FT_changed = true;
		break;
	case GLUT_KEY_DOWN:
		FT_inter = FT_inter - 0.1;
		FT_changed = true;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutCreateWindow("Auto-Surface(Extended1)");

	init();
	glutDisplayFunc(display_callback);
	glutReshapeFunc(reshape_callback);
	glutMouseFunc(mouse_callback);
	glutMotionFunc(mouse_move_callback);
	glutKeyboardFunc(keyboard_callback);
	glutSpecialFunc(specialkey);
	glutMainLoop();
	return 0;
}


