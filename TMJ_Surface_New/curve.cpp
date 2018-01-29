#pragma warning(disable:4146)
#pragma warning(disable:4996)

#include "curve.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <fstream>
#include <iostream>
#include <GL/glut.h>

using namespace std;

#define SET_VECTOR2(V, V1, V2)          do { (V)[X] = (V1); (V)[Y] = (V2); } while (0)
#define SET_VECTOR3(V, V1, V2, V3)          do { (V)[X] = (V1); (V)[Y] = (V2); (V)[Z] = (V3);} while (0)

#define VECTOR2_X_SCALA_ADD(O, V, S)    do { O[X] += (S) * (V)[X]; O[Y] += (S) * (V)[Y]; } while (0)
#define VECTOR3_X_SCALA_ADD(O, V, S)    do { O[X] += (S) * (V)[X]; O[Y] += (S) * (V)[Y]; O[Z] += (S) * (V)[Z]; } while (0)

#ifdef DEBUG
void PRINT_CTRLPTS(CubicBezierCurve* crv) {
	int i;
	printf("curve %p\n[\n", crv);
	for (i = 0; i<4; ++i)
		printf("[%f, %f]\n", crv->control_pts[i][X], crv->control_pts[i][Y]);
	printf("]\n");
}
#endif

REAL move_x = 0, move_y = 0, move_z = 0;

/*Cubic Bezier Curve*/
void CubicBezierCurve::EvaluateCurve2D(const REAL t, Point2 value)
{
	const REAL t_inv = 1.0f - t;
	const REAL t_inv_sq = t_inv * t_inv;
	const REAL t_sq = t * t;
	const REAL b0 = t_inv_sq * t_inv;
	const REAL b1 = 3 * t_inv_sq * t;
	const REAL b2 = 3 * t_inv * t_sq;
	const REAL b3 = t_sq * t;

	SET_VECTOR2(value, 0, 0);
	VECTOR2_X_SCALA_ADD(value, this->control_pts[0], b0);
	VECTOR2_X_SCALA_ADD(value, this->control_pts[1], b1);
	VECTOR2_X_SCALA_ADD(value, this->control_pts[2], b2);
	VECTOR2_X_SCALA_ADD(value, this->control_pts[3], b3);
}

void CubicBezierCurve::EvaluateCurve3D(const REAL t, Point3 value)
{
	const REAL t_inv = 1.0f - t;
	const REAL t_inv_sq = t_inv * t_inv;
	const REAL t_sq = t * t;
	const REAL b0 = t_inv_sq * t_inv;
	const REAL b1 = 3 * t_inv_sq * t;
	const REAL b2 = 3 * t_inv * t_sq;
	const REAL b3 = t_sq * t;

	SET_VECTOR3(value, 0, 0, 0);
	VECTOR3_X_SCALA_ADD(value, this->control_pts[0], b0);
	VECTOR3_X_SCALA_ADD(value, this->control_pts[1], b1);
	VECTOR3_X_SCALA_ADD(value, this->control_pts[2], b2);
	VECTOR3_X_SCALA_ADD(value, this->control_pts[3], b3);
}

void CubicBezierCurve::DrawCurve2D()
{
	int i;
	/* Curve property */
	glColor3ub(120, 120, 120);
	/* curve */
	Point3 pt;
	glBegin(GL_LINE_STRIP);
	for (i = 0; i <= RES; ++i) {	
		const REAL t = (REAL)i / (REAL)RES;
		EvaluateCurve3D(t, pt);
		glVertex2f(pt[Y], pt[Z]);
	}
	glEnd();

	/* control mesh */
	glColor3ub(255, 0, 0);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i<4; ++i) {
		REAL *pt = this->control_pts[i];
		glVertex2f(pt[Y], pt[Z]);
	}
	glEnd();

	/* control pts */
	glColor3ub(75, 119, 190);
	glPointSize(10.0);
	glBegin(GL_POINTS);
	for (i = 0; i < 4; ++i) {
		REAL *pt = this->control_pts[i];
		glVertex2f(pt[Y], pt[Z]);
	}
	glEnd();
}

void CubicBezierCurve::DrawCurve3D(bool red)
{
	int i;
	/* Curve property */
	glColor3ub(120, 120, 120);
	/* curve */
	if (red)
	{
		glLineWidth(5.0);
		glColor3ub(250, 227, 114);
	}

	Point3 pt;
	glBegin(GL_LINE_STRIP);
	for (i = 0; i <= RES; ++i) {
		const REAL t = (REAL)i / (REAL)RES;
		EvaluateCurve3D(t, pt);
		glVertex3f(pt[X], pt[Y], pt[Z]);
	}
	glEnd();
	glLineWidth(1.0);
	/* control mesh */
	glColor3ub(255, 0, 0);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i < 4; ++i) {
		REAL *pt = this->control_pts[i];
		glVertex3f(pt[X], pt[Y], pt[Z]);
	}
	glEnd();

	/* control pts */
	glColor3ub(75, 119, 190);
	glPointSize(8.0);
	glBegin(GL_POINTS);
	for (i = 0; i < 4; ++i) {
		REAL *pt = this->control_pts[i];
		glVertex3f(pt[X], pt[Y], pt[Z]);
	}
	glEnd();
}

int CubicBezierCurve::HitIndex2D(int x, int y)
{
	for (int i = 0; i < 4; ++i) {
		REAL tx = this->control_pts[i][0] - x;
		REAL ty = this->control_pts[i][1] - y;
		if ((tx * tx + ty * ty) < 30)
			return i;
	}
	return -1;
}

/*Cubic Bezier Curve*/
void BicubicBezierSurface::EvaluateSurface3D(const REAL t1, const REAL t2, Point3 value)
{
	const float t1_inv = 1.0f - t1;
	const float t1_inv_sq = t1_inv * t1_inv;
	const float t1_sq = t1 * t1;
	const float t2_inv = 1.0f - t2;
	const float t2_inv_sq = t2_inv * t2_inv;
	const float t2_sq = t2 * t2;

	float b[2][4];
	b[0][0] = t1_inv_sq * t1_inv;
	b[0][1] = 3 * t1_inv_sq * t1;
	b[0][2] = 3 * t1_inv * t1_sq;
	b[0][3] = t1_sq * t1;
	b[1][0] = t2_inv_sq * t2_inv;
	b[1][1] = 3 * t2_inv_sq * t2;
	b[1][2] = 3 * t2_inv * t2_sq;
	b[1][3] = t2_sq * t2;

	SET_PT3(value, 0, 0, 0);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			VECTOR3_X_SCALA_ADD(value, this->control_pts[i][j], b[0][i] * b[1][j]);
}

void BicubicBezierSurface::CalculateSurface3D()
{
	/*Calculate surface*/
	for (int i = 0; i <= RES; i++)
		for (int j = 0; j <= RES; j++)
		{
			this->EvaluateSurface3D(i / (float)RES, j / (float)RES, this->points[i][j]);
		}
}

void BicubicBezierSurface::DrawSurface3D()
{
	glColor3ub(250, 227, 113);
	for (int i = 0; i <= RES; i += 4)
	{
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j <= RES; j++)
			glVertex3f(this->points[i][j][0], this->points[i][j][1], this->points[i][j][2]);
		glEnd();
	}
	for (int i = 0; i <= RES; i += 4)
	{
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j <= RES; j++)
			glVertex3f(this->points[j][i][0], this->points[j][i][1], this->points[j][i][2]);
		glEnd();
	}

	glColor3ub(137, 157, 192);
	glBegin(GL_QUADS);
	for (int i = 0; i < RES; i++)
	{
		for (int j = 0; j < RES; j++)
		{
			glVertex3f(this->points[i][j][0], this->points[i][j][1], this->points[i][j][2]);
			glVertex3f(this->points[i + 1][j][0], this->points[i + 1][j][1], this->points[i + 1][j][2]);
			glVertex3f(this->points[i + 1][j + 1][0], this->points[i + 1][j + 1][1], this->points[i + 1][j + 1][2]);
			glVertex3f(this->points[i][j + 1][0], this->points[i][j + 1][1], this->points[i][j + 1][2]);
		}
	}
	glEnd();
}

/*Model*/
void Model::SetInfo(char *filename)
{
	int count = 0, count1 = 0, cv = 0;
	REAL f1, f2, f3, ymin, ymax;
	char buffer[256];
	fstream outfile;
	outfile.open(filename);

	while (!outfile.eof())
	{
		outfile.getline(buffer, 256, '\n'); //getline(char *,int,char)   
		{
			if (buffer[0] == 'v')
			{
				if (buffer[1] == ' ') //vertex:v ***
				{
					sscanf(buffer, "v %f %f %f", &f1, &f2, &f3); //easy way to get number from a string

					if (count == 0)
					{
						if (move_x == 0 && move_y == 0 && move_z == 0)
						{
							move_x = -f1; //ok
							move_y = -f2; //ok
							move_z = -f3; //ok
						}
						ymax = ymin = 0;
						count++;
					}

					this->vertex[cv][X] = f1 + move_x; //ok
					this->vertex[cv][Y] = f2 + move_y; //ok
					this->vertex[cv][Z] = f3 + move_z; //ok

					if (ymax < this->vertex[cv][Y])
						ymax = this->vertex[cv][Y];
					if (ymin > this->vertex[cv][Y])
						ymin = this->vertex[cv][Y];

					cv++;
				}
			}
		}
	}
	this->divide_y = (ymax + ymin) / 2.0;
	outfile.close();
}

void Model::InitModel(char *filename, bool divide)
{
	char buffer[256];
	int cv = 0, cvn = 0, cf = 0, count1 = 0, count2 = 0;
	int i1, i2, i3, i4, i5, i6;
	REAL f1, f2, f3;
	REAL y1, y2;

	fstream outfile;
	outfile.open(filename);

	Point3 temp_maxx1, temp_minx1, temp_maxx2, temp_minx2;
	Point3 temp_maxz1, temp_minz1, temp_maxz2, temp_minz2;

	//20180128
	this->SetInfo(filename);

	while (!outfile.eof())
	{
		outfile.getline(buffer, 256, '\n'); //getline(char *,int,char)   
		if (!divide)  //if no need to divide y
		{
			if (buffer[0] == 'v')
			{
				if (buffer[1] == ' ') //vertex:v ***
				{
					sscanf(buffer, "v %f %f %f", &f1, &f2, &f3); //easy way to get number from a string
					
					this->vertex[cv][X] = f1 + move_x;
					this->vertex[cv][Y] = f2 + move_y;
					this->vertex[cv][Z] = f3 + move_z;

					/*init max_x and min_x*/
					if (cv == 0)
					{
						COPY_PT(this->max_x, this->vertex[cv]);
						COPY_PT(this->min_x, this->vertex[cv]);
					}
					else
					{
						if (this->vertex[cv][X] < min_x[X])
							COPY_PT(this->min_x, this->vertex[cv]);
						if (this->vertex[cv][X] > max_x[X])
							COPY_PT(this->max_x, this->vertex[cv]);
					}
					cv++;
				}
			}
		}
		else  //if need to divide in y coordiante
		{
			if (buffer[0] == 'v')
			{
				if (buffer[1] == ' ') //vertex:v ***
				{
					sscanf(buffer, "v %f %f %f", &f1, &f2, &f3); //easy way to get number from a string
					
					this->vertex[cv][X] = f1 + move_x;
					this->vertex[cv][Y] = f2 + move_y;
					this->vertex[cv][Z] = f3 + move_z;

					if (divide_y > this->vertex[cv][Y])	//Search part which y<stand_y
					{
						if (count1 == 0)
						{
							COPY_PT(temp_maxx1, this->vertex[cv]);
							COPY_PT(temp_minx1, this->vertex[cv]);
							count1++;
						}
						else
						{
							if (this->vertex[cv][X] < temp_minx1[X])
								COPY_PT(temp_minx1, this->vertex[cv]);
							if (this->vertex[cv][X] > temp_maxx1[X])
								COPY_PT(temp_maxx1, this->vertex[cv]);
						}
						cv++;
					}
					else  //Search part which y>=stand_y
					{
						if (count2 == 0)
						{
							COPY_PT(temp_maxx2, this->vertex[cv]);
							COPY_PT(temp_minx2, this->vertex[cv]);
							count2++;
						}
						else
						{
							if (this->vertex[cv][X] < temp_minx2[X])
								COPY_PT(temp_minx2, this->vertex[cv]);
							if (this->vertex[cv][X] > temp_maxx2[X])
								COPY_PT(temp_maxx2, this->vertex[cv]);
						}
						cv++;
					}
				}
			}
		}
		if (buffer[0] == 'f')  //face:f ***
		{
			sscanf(buffer, "f %d//%d %d//%d %d//%d", &i1, &i2, &i3, &i4, &i5, &i6); //easy way to get fber from a
			this->face[cf][0] = i1 - 1;
			this->face[cf][1] = i3 - 1;
			this->face[cf][2] = i5 - 1;

			cf++;
		}
	}
	if (divide)
	{
		if (temp_minx1[X] > temp_minx2[X])
			COPY_PT(this->min_x, temp_minx1);
		else
			COPY_PT(this->min_x, temp_minx2);

		if (temp_maxx1[X] < temp_maxx2[X])
			COPY_PT(this->max_x, temp_maxx1);
		else
			COPY_PT(this->max_x, temp_maxx2);
	}

	this->v_num = cv;
	this->f_num = cf;

	this->interval = (this->max_x[X] - this->min_x[X]) / ((this->slice_num )* 1.0);

	cout << "--------------------------" << endl;
	cout << filename << "'s information:" << endl;
	cout << "min_x is " << min_x[0] << endl;
	cout << "max_x is " << max_x[0] << endl;
	cout << "interval is " << this->interval << endl;
	cout << "vertex number is " << cv << endl;
	cout << "face number is " << cf << endl;
	outfile.close();
}

void Model::DrawModel()
{
	for (int i = 0; i < f_num; i++)
	{
		glColor3f(0.8, 0.8, 0.8);
		glBegin(GL_TRIANGLES);
		glVertex3f(this->vertex[this->face[i][0]][0], this->vertex[this->face[i][0]][1], this->vertex[this->face[i][0]][2]);
		glVertex3f(this->vertex[this->face[i][1]][0], this->vertex[this->face[i][1]][1], this->vertex[this->face[i][1]][2]);
		glVertex3f(this->vertex[this->face[i][2]][0], this->vertex[this->face[i][2]][1], this->vertex[this->face[i][2]][2]);
		glEnd();

		glColor3f(0.5, 0.5, 0.5);
		glBegin(GL_LINE_STRIP);
		glVertex3f(this->vertex[this->face[i][0]][0], this->vertex[this->face[i][0]][1], this->vertex[this->face[i][0]][2]);
		glVertex3f(this->vertex[this->face[i][1]][0], this->vertex[this->face[i][1]][1], this->vertex[this->face[i][1]][2]);
		glVertex3f(this->vertex[this->face[i][2]][0], this->vertex[this->face[i][2]][1], this->vertex[this->face[i][2]][2]);
		glEnd();
	}
}

void Model::SetSlice()
{
	REAL ratio;
	REAL inters_y, inters_z;

	int res_num;

	/*All slice setting*/
	for (int count = 0; count < slice_num; count++)
	{
		res_num = 0;
		for (int i = 0; i < 10000; i++)
		{
			for (int j = 0; j < 3; j++)
				this->all_slice[count].vertex[i][j] = 0;
		}
		/*calculate cur_x*/
		REAL cur_x = this->min_x[0] + (1 + count) * this->interval;

		/*Set intersection result points in px*/
		for (int i = 0; i < this->f_num; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				/*Model*/
				if (this->inter_tri(this->face[i], j, cur_x))  //If there is a line intersection between triangle mesh and x
				{
					ratio = (cur_x * 1.0 - this->vertex[this->face[i][j]][X]) / (1.0 * this->vertex[this->face[i][(j + 1) % 3]][X] - 1.0 * this->vertex[this->face[i][j]][X]);
					inters_y = this->vertex[this->face[i][j]][Y] + ratio * (this->vertex[this->face[i][(j + 1) % 3]][Y] - this->vertex[this->face[i][j]][Y]);
					inters_z = this->vertex[this->face[i][j]][Z] + ratio * (this->vertex[this->face[i][(j + 1) % 3]][Z] - this->vertex[this->face[i][j]][Z]);
					{
						this->all_slice[count].vertex[res_num][X] = cur_x;
						this->all_slice[count].vertex[res_num][Y] = inters_y;
						this->all_slice[count].vertex[res_num][Z] = inters_z;
						res_num++;
					}
				}
			}
		}

		this->all_slice[count].v_num = res_num;
		this->all_slice[count].index = count;
		this->all_slice[count].cur_x = cur_x;

		/*set slice's 4 cur*/
		this->all_slice[count].SetSliceCurve();
		/*Set minz vertex and maxz vertex*/
		this->all_slice[count].SetSliceMM();
		this->all_slice[count].SetMaxzCurve(true);
		this->all_slice[count].SetMinzCurve(true);
	}
}

void Model::Divide_SetSlice(bool upy, REAL* FT_inter)
{
	REAL ratio;
	REAL inters_y, inters_z;

	int res_num;

	/*All slice setting*/
	for (int count = 0; count < slice_num; count++)
	{
		res_num = 0;
		for (int i = 0; i < 10000; i++)
		{
			for (int j = 0; j < 3; j++)
				this->all_slice[count].vertex[i][j] = 0;
		}
		
		/*calculate cur_x*/
		REAL cur_x = this->min_x[0] + (1 + count) * this->interval;

		/*Set intersection result points in px*/
		for (int i = 0; i < this->f_num; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				/*Model*/
				if (this->inter_tri(this->face[i], j, cur_x))  //If there is a line intersection between triangle mesh and x
				{
					ratio = (cur_x * 1.0 - this->vertex[this->face[i][j]][X]) / (1.0 * this->vertex[this->face[i][(j + 1) % 3]][X] - 1.0 * this->vertex[this->face[i][j]][X]);
					inters_y = this->vertex[this->face[i][j]][Y] + ratio * (this->vertex[this->face[i][(j + 1) % 3]][Y] - this->vertex[this->face[i][j]][Y]);
					inters_z = this->vertex[this->face[i][j]][Z] + ratio * (this->vertex[this->face[i][(j + 1) % 3]][Z] - this->vertex[this->face[i][j]][Z]);

					if (upy)
					{
						if (inters_y > divide_y - *FT_inter)
						{
							this->all_slice[count].vertex[res_num][X] = cur_x;
							this->all_slice[count].vertex[res_num][Y] = inters_y;
							this->all_slice[count].vertex[res_num][Z] = inters_z;
							res_num++;

						}
					}
					if (!upy)
					{
						if (inters_y < divide_y + *FT_inter)
						{
							this->all_slice[count].vertex[res_num][X] = cur_x;
							this->all_slice[count].vertex[res_num][Y] = inters_y;
							this->all_slice[count].vertex[res_num][Z] = inters_z;
							res_num++;
						}
					}
				}
			}
		}
		this->all_slice[count].v_num = res_num;
		this->all_slice[count].index = count;
		this->all_slice[count].cur_x = cur_x;

		/*set slice's 4 cur*/
		this->all_slice[count].SetSliceCurve();
		/*Set minz vertex and maxz vertex*/
		this->all_slice[count].SetSliceMM();
		this->all_slice[count].SetMaxzCurve(upy);
		this->all_slice[count].SetMinzCurve(upy);
	}
}

void Model::DrawSliceCurve(int index, int slice_index)
{
	bool temp = false;
	for (int i = 0; i < slice_num - 1; i++)
	{
		if (i == slice_index)
			temp = true;

		if (index == 1)
		{
			for (int j = 0; j < 4; j++)
				this->all_slice[i].cur[j].DrawCurve3D(temp);
		}
		else if (index == 2)
		{
			for (int j = 0; j < 2; j++)
			{
				this->all_slice[i].cur_maxz[j].DrawCurve3D(temp);
			//	this->all_slice[i].DrawMaxPt();
			//	this->all_slice[i].DrawResultPt();
			}
		}
		else if (index == 3)
		{
			for (int j = 0; j < 2; j++)
			{
				this->all_slice[i].cur_minz[j].DrawCurve3D(temp);
			//	this->all_slice[i].DrawMinPt();
			//	this->all_slice[i].DrawResultPt();
			}
		}
		temp = false;
	}
}

void Model::SetSurface()
{
	//Set model part surface
	//row
	for (int row = 0; row < (this->slice_num - 1) / 3; row++)
	{
		//column
		for (int col = 0; col < 4; col++)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					COPY_PT(this->sur[row][col].control_pts[i][j], this->all_slice[row * 3 + i].cur[col].control_pts[j]);
			//		cout << "sur[" << row << "][" << col << "].ctrlpt["<<i<<"]["<<j<<"] == all_slice[" << row * 3 + i << ".cur[" << col << "].control_pts[" << j << "]" << endl;
				}
			}
		}
	}

	/*Use minz vertex and maxz vertex to calculate surface*/
	for (int row = 0; row < (this->slice_num - 1) / 3; row++)
	{
		//column
		for (int col = 0; col < 2; col++)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					COPY_PT(this->maxz_sur[row][col].control_pts[i][j], this->all_slice[row * 3 + i].cur_maxz[col].control_pts[j]);
					COPY_PT(this->minz_sur[row][col].control_pts[i][j], this->all_slice[row * 3 + i].cur_minz[col].control_pts[j]);
				}
			}
		}
	}
}

void Model::DrawSliceSurface(int index)
{
	if (index == 1)
	{
		for (int i = 0; i < (this->slice_num - 1) / 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				this->sur[i][j].CalculateSurface3D();
				this->sur[i][j].DrawSurface3D();
			}
		}
	}
	else if (index == 2)
	{
		for (int i = 0; i < (this->slice_num - 1) / 3; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				this->maxz_sur[i][j].CalculateSurface3D();
				this->maxz_sur[i][j].DrawSurface3D();
			}
		}
	}
	else if (index == 3)
	{
		for (int i = 0; i < (this->slice_num - 1) / 3; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				this->minz_sur[i][j].CalculateSurface3D();
				this->minz_sur[i][j].DrawSurface3D();
			}
		}
	}
}

bool Model::inter_tri(int face[3], int i, float x)
{
	//face_1st line(0 1):
	if (((x - this->vertex[face[i]][X]) * (x - this->vertex[face[(i + 1) % 3]][X])) < 0)
		return true;
	return false;
}

void Model::OutFile(char *name, int index)
{
	/*cout << "out file succeed" << endl;
	char file[10];
	strcat(file, name);
	strcat(file, ".txt");
	cout << file << endl;*/

	//ofstream out1(name);
	//if (out1.is_open())
	//{
	//	out1 << "This is a Model file with 16 Bezier Surfaces" << endl;

	//	for (int i = 0; i < 4; i++)
	//	{
	//		for (int j = 0; j < 4; j++)
	//		{
	//			out1 << "Surface[" << i << "][" << j << "]'s ctrl pts" << endl;
	//			for(int m = 0; m < 4; m++)
	//				for (int n = 0; n < 4; n++)
	//				{
	//					out1 << this->sur[i][j].control_pts[m][n][X] + fx << "//";
	//					out1 << this->sur[i][j].control_pts[m][n][Y] + fy << "//";
	//					out1 << this->sur[i][j].control_pts[m][n][Z] + fz << endl;
	//				}
	//			out1 << endl;
	//		}
	//		out1 << endl;
	//	}
	//	out1.close();
	//}

	//ofstream out2("Model_quadmesh.txt");
	//if (out2.is_open())
	//{
	//	out2 << "This is a Model file in quad mesh" << endl;

	//	for (int i = 0; i < 4; i++)
	//	{
	//		for (int j = 0; j < 4; j++)
	//		{
	//			for (int m = 0; m < RES; m++)
	//				for (int n = 0; n < RES; n++)
	//				{
	//					out2 << "Quad:" << endl;
	//					out2 << this->sur[i][j].points[m][n][X] + fx << "//";
	//					out2 << this->sur[i][j].points[m][n][Y] + fy << "//";
	//					out2 << this->sur[i][j].points[m][n][Z] + fz << endl;

	//					out2 << this->sur[i][j].points[m + 1][n][X] + fx << "//";
	//					out2 << this->sur[i][j].points[m + 1][n][Y] + fy << "//";
	//					out2 << this->sur[i][j].points[m + 1][n][Z] + fz << endl;

	//					out2 << this->sur[i][j].points[m + 1][n + 1][X] + fx << "//";
	//					out2 << this->sur[i][j].points[m + 1][n + 1][Y] + fy << "//";
	//					out2 << this->sur[i][j].points[m + 1][n + 1][Z] + fz << endl;

	//					out2 << this->sur[i][j].points[m][n + 1][X] + fx << "//";
	//					out2 << this->sur[i][j].points[m][n + 1][Y] + fy << "//";
	//					out2 << this->sur[i][j].points[m][n + 1][Z] + fz << endl;
	//				}
	//			out2 << endl;
	//		}
	//		out2 << endl;
	//	}
	//	out2.close();
	//}

	/*ofstream out3(name);
	if (out3.is_open())
	{
		out3 << "This is "<< name <<" Model file in array shape" << endl;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				out3 << "Patch(17*17)[" << i << "][" << j << "]:" << endl;
				for (int m = 0; m <= RES; m++)
				{
					out3 << "Row." << m << endl;
					for (int n = 0; n <= RES; n++)
					{
						out3 << this->sur[i][j].points[m][n][X] - fx << "//";
						out3 << this->sur[i][j].points[m][n][Y] - fy << "//";
						out3 << this->sur[i][j].points[m][n][Z] - fz << endl;
					}
					out3 << endl;
				}
			}
			out3 << endl;
		}
		out3.close();
	}*/
	ofstream out4(name);
	if (out4.is_open())
	{
		out4 << "This is " << name << " Model file in array shape" << endl;

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				out4 << "Patch(17*17)[" << i << "][" << j << "]:" << endl;
				for (int m = 0; m <= RES; m++)
				{
					out4 << "Row." << m << endl;
					for (int n = 0; n <= RES; n++)
					{
						if (index == 0)
						{
							out4 << this->maxz_sur[i][j].points[m][n][X] - move_x << "//";
							out4 << this->maxz_sur[i][j].points[m][n][Y] - move_y << "//";
							out4 << this->maxz_sur[i][j].points[m][n][Z] - move_z << endl;
						}
						else if (index == 1)
						{
							out4 << this->minz_sur[i][j].points[m][n][X] - move_x << "//";
							out4 << this->minz_sur[i][j].points[m][n][Y] - move_y << "//";
							out4 << this->minz_sur[i][j].points[m][n][Z] - move_z << endl;
						}
					}
					out4 << endl;
				}
			}
			out4 << endl;
		}
		out4.close();
	}
}

/*SliceBoundary*/
void SliceBoundary::SetSliceCurve()
{
	REAL y_max, ymax_z, y_min, ymin_z, z_max, zmax_y, z_min, zmin_y, x;

	Point3 start[4], middle[4], end[4], ctrlp_2[4], ctrlp_3[4];

	z_max = z_min = ymax_z = ymin_z = this->vertex[0][Z];
	y_max = y_min = zmax_y = zmin_y = this->vertex[0][Y];
	x = this->cur_x;

	/*Find AABB Box*/
	for (int i = 1; i < this->v_num; i++) 
	{
		if (z_max <= this->vertex[i][Z]) {
			z_max = this->vertex[i][Z];
			zmax_y = this->vertex[i][Y];
		}

		if (z_min >= this->vertex[i][Z]) {
			z_min = this->vertex[i][Z];
			zmin_y = this->vertex[i][Y];
		}

		if (y_max <= this->vertex[i][Y]) {
			y_max = this->vertex[i][Y];
			ymax_z = this->vertex[i][Z];
		}

		if (y_min >= this->vertex[i][Y]) {
			y_min = this->vertex[i][Y];
			ymin_z = this->vertex[i][Z];
		}
	}
	/*curve1*/
	start[0][Y] = y_min;
	start[0][Z] = ymin_z;

	middle[0][Y] = y_min;
	middle[0][Z] = z_max;

	end[0][Y] = zmax_y;
	end[0][Z] = z_max;
	/*curve2*/
	start[1][Y] = zmax_y;
	start[1][Z] = z_max;

	middle[1][Y] = y_max;
	middle[1][Z] = z_max;

	end[1][Y] = y_max;
	end[1][Z] = ymax_z;
	/*curve3*/
	start[2][Y] = y_max;
	start[2][Z] = ymax_z;

	middle[2][Y] = y_max;
	middle[2][Z] = z_min;

	end[2][Y] = zmin_y;
	end[2][Z] = z_min;
	/*curve4*/
	start[3][Y] = zmin_y;
	start[3][Z] = z_min;

	middle[3][Y] = y_min;
	middle[3][Z] = z_min;

	end[3][Y] = y_min;
	end[3][Z] = ymin_z;

	/*With start as ctrpt1, ctrp2, ctrp3, end as ctrp4*/
	for (int i = 0; i < 4; i++) {
		ctrlp_2[i][Y] = 0.333*start[i][Y] + 0.666*middle[i][Y];
		ctrlp_2[i][Z] = 0.333*start[i][Z] + 0.666*middle[i][Z];

		ctrlp_3[i][Y] = 0.666*middle[i][Y] + 0.333*end[i][Y];
		ctrlp_3[i][Z] = 0.666*middle[i][Z] + 0.333*end[i][Z];

		SET_PT3(this->cur[i].control_pts[0], x, start[i][Y], start[i][Z]);
		SET_PT3(this->cur[i].control_pts[1], x, ctrlp_2[i][Y], ctrlp_2[i][Z]);
		SET_PT3(this->cur[i].control_pts[2], x, ctrlp_3[i][Y], ctrlp_3[i][Z]);
		SET_PT3(this->cur[i].control_pts[3], x, end[i][Y], end[i][Z]);
	}
	this->ymin = y_min;
	this->ymax = y_max;
	this->zmin = z_min;
	this->zmax = z_max;
}

/*SliceBoundary*/
void SliceBoundary::SetSliceMM()
{
	REAL interval, y1, y2;
	/*Set ymin and ymax*/
	{
		//interval = (all_slice[count].ymax - all_slice[count].ymin) / 10.0;
		interval = (this->ymax - this->ymin) / MV_num;
		
		for (int i = 1; i < MV_num + 1; i++)
		{
			y1 = this->ymin + (i - 1) * interval;
			y2 = this->ymin + i * interval;

			/*set slice's max points modified*/
			this->SetMaxz(y1, y2, &this->maxz_vertex[i - 1]);
			/*set slice's min points modified*/
			this->SetMinz(y1, y2, &this->minz_vertex[i - 1]);
			//cout << "maxPoint is: (" << this->maxz_vertex[i - 1][X] << "," << this->maxz_vertex[i - 1][Y] << "," << this->maxz_vertex[i - 1][Z] << ")" << endl;
		}
		cout << endl;
	}

}

void SliceBoundary::SetMaxz(REAL y_start, REAL y_end, Point3 *max)
{
	int count = 0;

	/*draw divide lines*/
	/*glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(this->cur_x, y_start, -5);
	glVertex3f(this->cur_x, y_start, 5);

	glVertex3f(this->cur_x, y_end, -5);
	glVertex3f(this->cur_x, y_end, 5);
	glEnd();*/

	for (int i = 0; i < this->v_num; i++)
	{

		if ((this->vertex[i][Y] > y_start) && (this->vertex[i][Y] < y_end))
		{
			if (count == 0)
			{
				COPY_PT(*max, this->vertex[i]);
				count++;
			}
			else
			{
				if (this->vertex[i][Z] > (*max)[Z])
					COPY_PT(*max, this->vertex[i]);
			}
		}
	}
}

void SliceBoundary::SetMinz(REAL y_start, REAL y_end, Point3 *min)
{
	int count = 0;

	/*draw divide lines*/
	/*glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex3f(this->cur_x, y_start, -5);
	glVertex3f(this->cur_x, y_start, 5);

	glVertex3f(this->cur_x, y_end, -5);
	glVertex3f(this->cur_x, y_end, 5);
	glEnd();*/

	for (int i = 0; i < this->v_num; i++)
	{
		if ((this->vertex[i][Y] > y_start) && (this->vertex[i][Y] < y_end))
		{
			if (count == 0)
			{
				COPY_PT(*min, this->vertex[i]);
				count++;
			}
			else
			{
				if (this->vertex[i][Z] < (*min)[Z])
					COPY_PT(*min, this->vertex[i]);
			}
		}
	}
}

void SliceBoundary::SetMaxzCurve(bool upz)
{
	REAL y_max, ymax_z, y_min, ymin_z, z_max, zmax_y, z_min, zmin_y, x;
	x = this->cur_x;

	Point3 start[2], middle[2], end[2], ctrlp_2[2], ctrlp_3[2];
	z_max = z_min = ymax_z = ymin_z = this->maxz_vertex[0][Z];
	y_max = y_min = zmax_y = zmin_y = this->maxz_vertex[0][Y];

	for (int i = 0; i < MV_num; i++)
	{
		if (z_max <= this->maxz_vertex[i][Z]) {
			z_max = this->maxz_vertex[i][Z];
			zmax_y = this->maxz_vertex[i][Y];
		}

		if (z_min >= this->maxz_vertex[i][Z]) {
			z_min = this->maxz_vertex[i][Z];
			zmin_y = this->maxz_vertex[i][Y];
		}

		if (y_max <= this->maxz_vertex[i][Y]) {
			y_max = this->maxz_vertex[i][Y];
			ymax_z = this->maxz_vertex[i][Z];
		}

		if (y_min >= this->maxz_vertex[i][Y]) {
			y_min = this->maxz_vertex[i][Y];
			ymin_z = this->maxz_vertex[i][Z];
		}
	}
	if (upz)
	{
		/*curve1*/
		start[0][Y] = y_min;
		start[0][Z] = ymin_z;

		middle[0][Y] = y_min;
		middle[0][Z] = z_max;

		end[0][Y] = zmax_y;
		end[0][Z] = z_max;
		/*curve2*/
		start[1][Y] = zmax_y;
		start[1][Z] = z_max;

		middle[1][Y] = y_max;
		middle[1][Z] = z_max;

		end[1][Y] = y_max;
		end[1][Z] = ymax_z;
	}
	else
	{
		/*curve1*/
		start[0][Y] = y_max;
		start[0][Z] = ymax_z;

		middle[0][Y] = y_max;
		middle[0][Z] = z_min;

		end[0][Y] = zmin_y;
		end[0][Z] = z_min;
		/*curve2*/
		start[1][Y] = zmin_y;
		start[1][Z] = z_min;

		middle[1][Y] = y_min;
		middle[1][Z] = z_min;

		end[1][Y] = y_min;
		end[1][Z] = ymin_z;
	}
	/*With start as ctrpt1, ctrp2, ctrp3, end as ctrp4*/
	for (int i = 0; i < 2; i++) {
		ctrlp_2[i][Y] = 0.333*start[i][Y] + 0.666*middle[i][Y];
		ctrlp_2[i][Z] = 0.333*start[i][Z] + 0.666*middle[i][Z];

		ctrlp_3[i][Y] = 0.666*middle[i][Y] + 0.333*end[i][Y];
		ctrlp_3[i][Z] = 0.666*middle[i][Z] + 0.333*end[i][Z];

		SET_PT3(this->cur_maxz[i].control_pts[0], x, start[i][Y], start[i][Z]);
		SET_PT3(this->cur_maxz[i].control_pts[1], x, ctrlp_2[i][Y], ctrlp_2[i][Z]);
		SET_PT3(this->cur_maxz[i].control_pts[2], x, ctrlp_3[i][Y], ctrlp_3[i][Z]);
		SET_PT3(this->cur_maxz[i].control_pts[3], x, end[i][Y], end[i][Z]);
	}
}

void SliceBoundary::SetMinzCurve(bool upz)
{
	REAL y_max, ymax_z, y_min, ymin_z, z_max, zmax_y, z_min, zmin_y, x;
	x = this->cur_x;

	Point3 start[2], middle[2], end[2], ctrlp_2[2], ctrlp_3[2];
	z_max = z_min = ymax_z = ymin_z = this->minz_vertex[0][Z];
	y_max = y_min = zmax_y = zmin_y = this->minz_vertex[0][Y];

	for (int i = 0; i < MV_num; i++)
	{
		if (z_max <= this->minz_vertex[i][Z]) {
			z_max = this->minz_vertex[i][Z];
			zmax_y = this->minz_vertex[i][Y];
		}

		if (z_min >= this->minz_vertex[i][Z]) {
			z_min = this->minz_vertex[i][Z];
			zmin_y = this->minz_vertex[i][Y];
		}

		if (y_max <= this->minz_vertex[i][Y]) {
			y_max = this->minz_vertex[i][Y];
			ymax_z = this->minz_vertex[i][Z];
		}

		if (y_min >= this->minz_vertex[i][Y]) {
			y_min = this->minz_vertex[i][Y];
			ymin_z = this->minz_vertex[i][Z];
		}
	}
	if (upz)
	{
		/*curve1*/
		start[0][Y] = y_min;
		start[0][Z] = ymin_z;

		middle[0][Y] = y_min;
		middle[0][Z] = z_max;

		end[0][Y] = zmax_y;
		end[0][Z] = z_max;
		/*curve2*/
		start[1][Y] = zmax_y;
		start[1][Z] = z_max;

		middle[1][Y] = y_max;
		middle[1][Z] = z_max;

		end[1][Y] = y_max;
		end[1][Z] = ymax_z;
	}
	else
	{
		/*curve1*/
		start[0][Y] = y_max;
		start[0][Z] = ymax_z;

		middle[0][Y] = y_max;
		middle[0][Z] = z_min;

		end[0][Y] = zmin_y;
		end[0][Z] = z_min;
		/*curve2*/
		start[1][Y] = zmin_y;
		start[1][Z] = z_min;

		middle[1][Y] = y_min;
		middle[1][Z] = z_min;

		end[1][Y] = y_min;
		end[1][Z] = ymin_z;
	}
	/*With start as ctrpt1, ctrp2, ctrp3, end as ctrp4*/
	for (int i = 0; i < 2; i++) {
		ctrlp_2[i][Y] = 0.333*start[i][Y] + 0.666*middle[i][Y];
		ctrlp_2[i][Z] = 0.333*start[i][Z] + 0.666*middle[i][Z];

		ctrlp_3[i][Y] = 0.666*middle[i][Y] + 0.333*end[i][Y];
		ctrlp_3[i][Z] = 0.666*middle[i][Z] + 0.333*end[i][Z];

		SET_PT3(this->cur_minz[i].control_pts[0], x, start[i][Y], start[i][Z]);
		SET_PT3(this->cur_minz[i].control_pts[1], x, ctrlp_2[i][Y], ctrlp_2[i][Z]);
		SET_PT3(this->cur_minz[i].control_pts[2], x, ctrlp_3[i][Y], ctrlp_3[i][Z]);
		SET_PT3(this->cur_minz[i].control_pts[3], x, end[i][Y], end[i][Z]);
	}
}

void SliceBoundary::DrawResultPt()
{
	glColor3ub(236, 236, 236);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < this->v_num; i++)
		glVertex3fv(this->vertex[i]);
	glEnd();
}

void SliceBoundary::DrawMaxPt()
{
	glColor3ub(255, 0, 0);
	glPointSize(9.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < MV_num; i++)
		glVertex3fv(this->maxz_vertex[i]);
	glEnd();
}

void SliceBoundary::DrawMinPt()
{
	glColor3ub(0, 255, 0);
	glPointSize(9.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < MV_num; i++)
		glVertex3fv(this->minz_vertex[i]);
	glEnd();
}