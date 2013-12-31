#include "StdAfx.h"
#include "shape.h"
#include <math.h>

#pragma warning (disable:4244) //conversion from 'DOUBLE' to 'LONG', possible loss of data

CString ShapeNames[NUM_SHAPE_TYPES] = {_T("Point"), _T("Multipoint"), _T("Arc"), _T("Polygon")};

INT Round(DOUBLE x) {
	if(x - floor(x) >= 0.5) 
		return (INT) ceil(x);
	else 
		return (INT) x;
}

// --------------------------------------------------------
//				Определение класса CShape

// --------------------------------------------------------
//				Определение класса CDot

CDot::CDot() : m_dX(-1), m_dY(-1) {}

CDot::CDot(DOUBLE dX, DOUBLE dY) : m_dX(dX), m_dY(dY) {}

CDot::~CDot() {}

void CDot::Draw(CDC &dc, DOUBLE dScale, COLORREF Color) {
	dc.SetPixelV(Round(m_dX * dScale), Round(m_dY * dScale), Color);
}

CDot& CDot::operator =(CDot &dot) {
	m_dX = dot.GetX();
	m_dY = dot.GetY();
	return *this;
}

__inline DOUBLE CDot::GetX() {
	return m_dX;
}

__inline void CDot::SetX(DOUBLE dNewX) {
	m_dX = dNewX;
}

__inline DOUBLE CDot::GetY() {
	return m_dY;
}

__inline void CDot::SetY(DOUBLE dNewY) {
	m_dY = dNewY;
}

// --------------------------------------------------------
//				Определение класса СMultiDot
CMultiDot::~CMultiDot() {
	if(m_Points != NULL) delete[] m_Points;
}

void CMultiDot::Draw(CDC &dc, DOUBLE dScale, COLORREF Color) {
	for(int i = 0; i < m_iNumPoints; i++)
		m_Points[i].Draw(dc, dScale, Color);
}

// --------------------------------------------------------
//				Определение класса CArc

CArc::CArc(CMyRect &BoundBox, INT *Parts, INT NumParts, CDot *Points, INT NumPoints) {
	ASSERT(Parts);
	ASSERT(Points);

	m_Box = BoundBox;
	m_iNumParts = NumParts;
	m_iParts = new INT[NumParts];
	CopyMemory(m_iParts, Parts, sizeof(*Parts) * NumParts);
	m_iNumPoints = NumPoints;
	m_Points = new CDot[NumPoints];
	CopyMemory(m_Points, Points, sizeof(*Points) * NumPoints);
}

CArc::~CArc() {
	delete[] m_iParts;
	delete[] m_Points;
}

void CArc::Draw(CDC &dc, DOUBLE dScale, COLORREF Color) {
	int LineLen, i, j;
	LPPOINT Points;

	for(i = 0; i < m_iNumParts; i++) {
		LineLen = i < m_iNumParts - 1 ? m_iParts[i+1] - m_iParts[i] : m_iNumPoints - m_iParts[i];
		Points = new POINT[LineLen];
		for(j = 0; j < LineLen; j++) {
			Points[j].x = (LONG) m_Points[m_iParts[i] + j].GetX() * dScale;
			Points[j].y = (LONG) m_Points[m_iParts[i] + j].GetY() * dScale;
		}
		dc.Polyline(Points, LineLen);
		delete[] Points;
	}
}

// --------------------------------------------------------
//				Определение класса CPolygon
CPolygon::CPolygon(CMyRect &BoundBox, INT *Parts, INT NumParts, 
				   CDot *Points, INT NumPoints) 
{
	ASSERT(Parts);
	ASSERT(Points);

	m_Box = BoundBox;
	m_iNumParts = NumParts;
	m_iParts = new INT[NumParts];
	CopyMemory(m_iParts, Parts, sizeof(*Parts) * NumParts);
	m_iNumPoints = NumPoints;
	m_Points = new CDot[NumPoints];
	CopyMemory(m_Points, Points, sizeof(*Points) * NumPoints);
}

CPolygon::~CPolygon() {
	delete[] m_iParts;
	delete[] m_Points;
}

void CPolygon::Draw(CDC &dc, DOUBLE dScale, COLORREF Color) {
	int LineLen, i, j;
	LPPOINT Points;

	for(i = 0; i < m_iNumParts; i++) {
		LineLen = i < m_iNumParts - 1 ? m_iParts[i+1] - m_iParts[i] : m_iNumPoints - m_iParts[i];
		Points = new POINT[LineLen];
		for(j = 0; j < LineLen; j++) {
			Points[j].x = (LONG) m_Points[m_iParts[i] + j].GetX() * dScale;
			Points[j].y = (LONG) m_Points[m_iParts[i] + j].GetY() * dScale;
		}
		dc.Polygon(Points, LineLen);
		delete[] Points;
	}
}


// --------------------------------------------------------
// --------------------------------------------------------
// --------------------------------------------------------
// ВСПОМОГАТЕЛЬНЫЕ КЛАССЫ

CMyRect::CMyRect() : m_dXMin(0), m_dYMin(0), m_dXMax(0), m_dYMax(0) 
{
}

CMyRect::CMyRect(DOUBLE dXMin, DOUBLE dYMin, DOUBLE dXMax, DOUBLE dYMax) :
m_dXMin(dXMin), m_dYMin(dYMin), m_dXMax(dXMax), m_dYMax(dYMax)
{
}

CMyRect::CMyRect(CRect &r) {
	m_dXMin = r.TopLeft().x;
	m_dYMin = r.TopLeft().y;
	m_dXMax = r.BottomRight().x;
	m_dYMax = r.BottomRight().y;
}

// --------------------------------------------------------

BOOL CMyRect::PointInRect(DOUBLE dX, DOUBLE dY) {
	return	dX < m_dXMin || dX > m_dXMax || 
			dY < m_dYMin || dY > m_dYMax ? FALSE : TRUE;
}

// --------------------------------------------------------

BOOL CMyRect::InRect(CMyRect &OuterRect) {
	if(	OuterRect.PointInRect(m_dXMin, m_dYMin) &&
		OuterRect.PointInRect(m_dXMax, m_dYMax))
		return TRUE;
	else
		return FALSE;
}

// --------------------------------------------------------
void  CMyRect::SetRect(DOUBLE dXMin, DOUBLE dYMin, DOUBLE dXMax, DOUBLE dYMax) {
	m_dXMin = dXMin;
	m_dYMin = dYMin;
	m_dXMax = dXMax;
	m_dYMax = dYMax;
}