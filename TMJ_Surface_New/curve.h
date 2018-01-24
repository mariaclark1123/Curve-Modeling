#include <vector>

#ifndef _CURVE_H_
#define _CURVE_H_

#define PRECISION   1e-5
#define EPS         1e-6        /* data type is float */
#define RES			16
#define SNUM		21

typedef float REAL;

typedef REAL Point3[3];
typedef REAL Point2[2];
typedef int Face[3];

class CubicBezierCurve
{
public:
	Point3 control_pts[4];

	void EvaluateCurve2D(const REAL t, Point2 value);
	void EvaluateCurve3D(const REAL t, Point3 value);
	void DrawCurve2D();
	void DrawCurve3D(bool red);
	int HitIndex2D(int x, int y);
};

class BicubicBezierSurface
{
public:
	Point3 control_pts[4][4];
	Point3 points[RES + 1][RES + 1];

	void EvaluateSurface3D(const REAL t1, const REAL t2, Point3 value);
	void CalculateSurface3D();
	void DrawSurface3D();
};

class SliceBoundary {
public:
	/*Assignment from Model::SetSlice(REAL z, bool upz)*/
	int v_num; 
	int index;
	REAL cur_x;

	/*Assignment from SliceBoundary::SetBound()*/
	Point3 vertex[10000];
	CubicBezierCurve cur[4];          //one slice boundary consists of 4 curves

	/*Assignment from SliceBoundary::SetMaxz()*/
	Point3 maxz_vertex[5];
	CubicBezierCurve cur_maxz[2];
	
	/*Assignment from SliceBoundary::SetMinz*/
	Point3 minz_vertex[5];
	CubicBezierCurve cur_minz[2];

	REAL ymin, ymax, zmin, zmax;      //draw the bound box 

	/*Used in Model::SetSlice()*/
	void SetSliceCurve();
	void SetSliceMM();

	/*Set maxvertex and minvertex - in SetSlice and DivideSetSlice*/
	void SetMaxz(REAL y_start, REAL y_end, Point3 *max);
	void SetMinz(REAL y_start, REAL y_end, Point3 *min);

	/*Set maxcur[2] and mincur[2]*/
	void SetMaxzCurve(bool upz);
	void SetMinzCurve(bool upz);

	void DrawResultPt();
	void DrawMaxPt();
	void DrawMinPt();
};

class Model
{
public:	
	int slice_num = 14; /*only use 13 slice to make surface*/
	
	/*Assignment from Model::InitModel(char *filename, float fx, float fy, float fz)*/
	int v_num;
	int f_num;
	Point3 vertex[100000];
	Face face[200000];
	/*Calculate interval*/
	Point3 max_x; 
	Point3 min_x;
	REAL interval,fx, fy, fz;

	/*Slice information*/
	SliceBoundary cur_slice; /*noused*/
	SliceBoundary all_slice[14];
	BicubicBezierSurface sur[4][4];  //Assignement from SetSurface()
	BicubicBezierSurface maxz_sur[4][2];  //Assignement from SetSurface()
	BicubicBezierSurface minz_sur[4][2];  //Assignement from SetSurface()

	void InitModel(char *filename, float fx, float fy, float fz);
	void DrawModel();
	
	/*Set slice's inter-vertex number, index, cur_x*/
	void SetSlice();   
	void Divide_SetSlice(REAL y, bool upy);
	
	/*Draw all slice's bezier curve*/
	void DrawSliceCurve(int index, int slice_i);  

	void SetSurface(); 
	void DrawSliceSurface(int index);  //Draw all surfaces
	
	void OutFile(char *name, int index);

	bool inter_tri(int face[3], int i, float x);
};

enum XYZ { X = 0, Y, Z };

#define SET_PT2(V, V1, V2) do { (V)[0] = (V1); (V)[1] = (V2); } while (0)
#define SET_PT3(V, V1, V2, V3) do { (V)[0] = (V1); (V)[1] = (V2); (V)[2] = (V3); } while (0)
#define COPY_PT(V1,V2) do { (V1)[0] = (V2)[0]; (V1)[1] = (V2)[1]; (V1)[2] = (V2)[2]; } while (0)
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#endif /* _CURVE_H_ */