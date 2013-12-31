#pragma once

#include <afxdb.h>          // MFC database support
#include <deque>
#include "Shape.h"

using namespace std;

/* �������������� �����. ������� �� ������� ���������� �� 
������� ������ � ���� �� �����. 
����� - ���������� �������, � ������� ��������� ������������.
������������ ����� ����� � ���� ���� */
struct SMap {
	SMap() {
		m_Color = RGB(rand() % 200, rand() % 200, rand() % 200);
		m_bVisible = TRUE;
		m_IsDBUsed = FALSE;
	}

	~SMap() {
		//if(m_DB.IsOpen()) m_DB.Close();
	}

	/* ������� �� ���������� �� �������, ������������� ��
	�������� ������������ CShape.
	����� ��������� ��� ��������� �����.*/
	deque<CShape*> m_Shapes;

	// ������ ������ ���� �������� �����
	INT m_iShapeType;

	// ����, ������� ���� �������� ����� �� ������
	COLORREF m_Color;

	/* ���������, ���� �� ������������ ����� �� ������ ��� ���.
	��� ����� �� ������� �� ��, ��� ��������� ����� ���������� �����
	��������� ���� � ���� �����.*/
	BOOL m_bVisible;

	// �������������� �������������
	CMyRect m_BoundBox;

	/* ����������, ������������ �� ������ � shape-������ ���� ������,
	����� ���������� ������������ �������������� ����������.
	�� ������������ �����, ����� � ����� � ����������� shp-������ 
	���� bdf-���� � ����� �� ������*/
	BOOL m_IsDBUsed;

	/* ���� ������, � ������� ������� �����.
	���� m_IsDBUsed == TRUE, ���������� ��������� � ��,
	���� FALSE, �� �� ��� ������ ����� ��� � � ���� ����� �������� ������*/
	//CDatabase m_DB;
};

// --------------------------------------------------------------
/*����� ���������. �������� ��� ������ �� ���� �������� ������,
�������� �� �������� shape-������.*/
class CShapeViewerDoc : public CDocument {
public:
	virtual ~CShapeViewerDoc();

	deque<SMap>*	GetLayers() {return &m_Layers;}

	CMyRect&		GetBoundingBox() {return m_BoundBox;}
	void			SetBoundingBox(const CMyRect &NewBBox) {m_BoundBox = NewBBox;}

	// ���������� �������� ������ ���������
	virtual BOOL OnNewDocument();
	// ��������� �������� shape-�����
	virtual void Serialize(CArchive& ar);
	// ������� ��� ���������� ���������
	virtual void DeleteContents();

	/* ��������� ������ �� Shape ����� � ��������� SMap.
	��� ������� �������� �����. TRUE, ��� ������ FALSE*/
	BOOL LoadFromFile(CArchive &ar, SMap &Layer, BOOL *pbAbortLoad);

private:
	CShapeViewerDoc();

	/* ������ ������ (�������� � ����������) � ������� ������� � �����
	� ��������� SMap.
    ��� �-�� ��������������� ��� LoadFromFile
	������� ������� ����� ������ ���� ����������� �� ������ ������ */
	CShape* _ReadShape(CFile &file, SMap &Layer, BOOL *pbAbortLoad);

	/* ������ �� ������� ������� ��������� 
	����� ������� ����� � �������  Big-Indian 
	���� ��������� ����� ����� ��������� ���������� CFileException*/
	DWORD _ReadBigIndianDWORD(CFile &f);

	/* ������ �� ������� ������� ��������� 
	����� ������� ����� � �������  Little-Indian 
	���� ��������� ����� ����� ��������� ���������� CFileException*/
	DWORD _ReadLittleIndianDWORD(CFile &f);

	/*������ �� ������� ������� ��������� 
	����� � �������  Little-Indian ���������� ���� 64-bit IEEE double*/
	DOUBLE _ReadLittleIndianDOUBLE(CFile &f);

	/* ������ �� ������� ������� ��������� �����
	���������� ������ c ������� ���� Point
	��� ������ 
	1 ���������� NULL
	2 ���������� ������ � ������� � �������: �������� �������� ��� ���
	2 ���� ����������� ������ �������� �������� ����� *pAbortLoad == TRUE,
	  ���� ��� - FALSE*/
	CDot* _ReadDot(CFile &f, SMap &Layer, INT iRecordNumber, 
					INT iContentLen, BOOL *pbAbortLoad, BOOL bSkipShapeType = FALSE);

	/* ������ �� ������� ������� ��������� �����
	���������� ������ c ������� ���� Arc ��� Polygon,
	� ����������� �� �������� ShapeT
	��� ������ 
	1 ���������� NULL
	2 ���������� ������ � ������� � �������: �������� �������� ��� ���
	2 ���� ����������� ������ �������� �������� ����� *pAbortLoad == TRUE,
	  ���� ��� - FALSE*/
	CShape* _ReadArcPoly(CFile &f, SMap &Layer, EShapeType ShapeT, 
						INT iRecordNumber, INT iContentLen, BOOL *pbAbortLoad);

	/*��������������� �-�� ��� �-�� Read���������
	������ ���� Shape Type � ���������� ������� ������
	���� ��������� ��� ������ ������� St �������� �� ������ � �����. -1
	��� ������ ���������� -1*/
	INT _ReadShapeType(CFile &f, EShapeType St, INT iRecordNumber, INT iContentLen);

	/*��������������� �-�� ��� �-�� Read���������
	������ ���� Bounding Box � ���������� ������� ������ ������
	��� ������ ���������� FALSE, ����� - TRUE
	���� ������ ��������� ��� Bounding Box'� ����� �������
	����������������� ��������� */
	BOOL _ReadBoundingBox(CFile &f, INT iRecordNumber, INT iContentLen, SMap &Layer, CMyRect *pBBox);

	/* ������� �� ����� ��������� �� ������ � �������� �����. 
	���� ������ �����������(IsCritical == TRUE), �� 
	������ ���������� ��������� � ������� OK � ������ �����. FALSE
	���� ����� �� ��������� (IsCritical == FALSE), 
	�� ���������� ������ Yes � No � � ����� ��������� �����������
	"�������� �� �������� ��� ���?"
	���� ������������ �������� Yes �����. TRUE, No - FALSE*/
	BOOL _ReportError(CString ErMsg, UINT CaptionID, BOOL IsCritical = FALSE);

	/*������� �����, ��������� ���������� ������ ��-���
	����������� ���������� ��������*/
	void _DeleteShapes(SMap *pLayer);

	void _ConnectToDB(CFile &f, SMap *pLayer);

private:
	// ������� �� ����� �����������. ������ ��� ��������� �������.
	deque<SMap> m_Layers;

	/*Bounding Box, ������� ������������ ��� ����� � ���������
	�� ����� �����-������� �� ������-������*/
	CMyRect m_BoundBox;

	DECLARE_DYNCREATE(CShapeViewerDoc)
	DECLARE_MESSAGE_MAP()
};