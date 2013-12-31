// Содержит объявления классов, представляюших геометрические фигуры

#pragma once
#include "afx.h"
#include "GlobalData.h"

#define NUM_SHAPE_TYPES 4	// кол-во типов фигур

/*Типы фигур встречающихся shape-файле. 
Значения этих констант соответствуют значению поля Shape Type
в спецификации shape-файла*/
enum EShapeType {
	ST_POINT = 1,
	ST_MULTIPOINT = 8,
	ST_ARC = 3,
	ST_POLYGON = 5
};

extern CString ShapeNames[]; // названия всех типов фигур

class CShape;

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------
// ВСПОМОГАТЕЛЬНЫЕ КЛАССЫ

// класс прямоугольника
class CMyRect {
public:
	CMyRect();
	CMyRect(DOUBLE dXMin, DOUBLE dYMin, DOUBLE dXMax, DOUBLE dYMax);
	CMyRect(CRect &r);

public:
	DOUBLE GetXMin() const {return m_dXMin;}
	void SetXMin(DOUBLE dVal) {m_dXMin = dVal;}

	DOUBLE GetYMin() const {return m_dYMin;}
	void SetYMin(DOUBLE dVal) {m_dYMin = dVal;}

	DOUBLE GetXMax() const {return m_dXMax;}
	void SetXMax(DOUBLE dVal) {m_dXMax = dVal;}

	DOUBLE GetYMax() const {return m_dYMax;}
	 void SetYMax(DOUBLE dVal) {m_dYMax = dVal;}

	 DOUBLE GetWidth() const {return m_dXMax - m_dXMin;}
	 DOUBLE GetHeight() const {return m_dYMax - m_dYMin;}

	void SetRect(DOUBLE dXMin, DOUBLE dYMin, DOUBLE dXMax, DOUBLE dYMax);

public:
	/* Проверяет, находится ли точка внутри или на границе прямоугольника.
	Если да возвращает TRUE, нет - FALSE*/
	BOOL PointInRect(DOUBLE dX, DOUBLE dY);

	/*Проверяет, находится ли прямоугольник внутри прямоугольника OuterRect
	Если да - возвр. TRUE, нет - FALSE*/
	BOOL InRect(CMyRect &OuterRect);

public:
	operator CRect() {
		CRect r;
		r.left = Round(m_dXMin);
		r.top = Round(m_dYMin);
		r.right = Round(m_dXMax);
		r.bottom = Round(m_dYMax);
		return r;
	}

private:
	DOUBLE m_dXMin, m_dYMin, m_dXMax, m_dYMax;
};

// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------
// КЛАССЫ ФИГУР

// --------------------------------------------------------

// Базовый абстрактный класс, от которого наследуются классы всех фигур
class CShape {
public:
	virtual ~CShape() {};

	// Отрисовка фигуры на контексте устройства
	virtual void Draw(CDC &dc, DOUBLE dScale, COLORREF Color) = 0;
};

// --------------------------------------------------------

class CDot : public CShape {
public:
	CDot();
	CDot(DOUBLE dX, DOUBLE dY);
	virtual ~CDot();

public:
	virtual void Draw(CDC &dc, DOUBLE dScale, COLORREF Color);

	DOUBLE GetX();
	void SetX(DOUBLE dNewX);

	DOUBLE GetY();
	void SetY(DOUBLE dNewY);

public:
	virtual CDot& operator =(CDot &dot);

private:
	DOUBLE m_dX, m_dY;
};

// --------------------------------------------------------

class CMultiDot : public CShape {
public:
	virtual ~CMultiDot(void);

	virtual void Draw(CDC &dc, DOUBLE dScale, COLORREF Color);

private:
	CMyRect	m_Box;
	INT		m_iNumPoints;
	CDot	*m_Points;
};

// --------------------------------------------------------

class CArc : public CShape {
public:
	CArc(CMyRect &BoundBox, INT *Parts, INT NumParts, CDot *Points, INT NumPoints);
	virtual ~CArc();
public:
	virtual void Draw(CDC &dc, DOUBLE dScale, COLORREF Color);

private:
	CMyRect		m_Box;				//Bounding Box
	INT			m_iNumParts;			//Number of Parts
	INT			m_iNumPoints;		//Total Number of Points

	/* An array of length NumParts. Stores for each polyline the index of 
	its first points in the points array. Array indexes are 
	with respect to 0. */
	INT			*m_iParts;			//Index to first Point in Part

	/* An array of length NumPoints. The points for each PolyLine in 
	the arc are stored end to end. The points for PolyLine 2 follow the
	points for PolyLine 1, and so on. The parts array holds the array index 
	of the starting point for each PolyLine. There is no delimiter in 
	the points array between PolyLines. */
	CDot		*m_Points;			//points for all parts
};

// --------------------------------------------------------

/* A polygon consists of a number of rings. 
A ring is a closed, non-self-intersecting loop. 
The order of verticles or orientation for a ring indicates which 
side of an observer walking along the ring in vertex order 
is the neighbourhood inside the polygon. 
Verticles for a single, ringed polygon are, therefore, 
always in clockwise order. The rings of a polygon are referred to as its parts.
Notes:
•	The rings are closed (the first and last vertex of a ring MUST be the same). 
•	The order of rings in the points array is not significant.
•	Polygons stored in a Shape File must be clean. The rings of a polygon cannot 
	have segments that intersect each other. In other words, a segment belonging 
	to one ring may not intersect a segment belonging to another ring. The rings 
	of a polygon can touch each other at vertices, but not along segments.
*/
class CPolygon : public CShape {
public:
	CPolygon(CMyRect &BoundBox, INT *Parts, INT NumParts, CDot *Points, INT NumPoints);
	virtual ~CPolygon();

public:
	virtual void Draw(CDC &dc, DOUBLE dScale, COLORREF Color);

private:
	CMyRect m_Box;		//Bounding Box
	INT m_iNumParts;		//Number of Parts
	INT m_iNumPoints;	//Total Number of Points
	INT *m_iParts;		//Index to first Point in Part
	CDot *m_Points;		//Points for all Parts
};
