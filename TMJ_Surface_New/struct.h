struct Judge_Model
{
	/*Done*/
	bool showModel = true;
	bool creatBezierSur = false;
	bool creatBSplineSur = false;
	bool planeMove = false;
	
	/*To do*/
	bool approxCur = false;
	bool outputQuad = false;
	bool outputSur = false;
};

struct Judge_Mouse
{
	bool isDragging = false;
	bool inBoard1 = false;
	bool inBoard2 = false;
};