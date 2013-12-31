#pragma once

/* ����� �������������. �������� �� ����������� ����� � ����
� ���������������� ��������� ��� ������ � ������ */
class CShapeViewerView : public CScrollView {
public:
	virtual ~CShapeViewerView();

private: // create from serialization only
	CShapeViewerView();
	DECLARE_DYNCREATE(CShapeViewerView)

private: // ������������� ������
	// ��������� ����
	virtual void OnDraw(CDC* pDC);

	/* ������ ������ scroll bar'� � ������������ �
	�������� ����� � ��������� */
	void UpdateScrollBars();

	/* �-�� ��� ����� � ����������. ���������� ��������� �� ��������.*/
	CShapeViewerDoc* GetDocument() const;

	/* ���������� ����� �������� ��� �������������,
	� ������� CDocument::UpdateAllViews � �����, ���� ���������� 
	��� ��������� ��������� �� ������ */
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
private: // ������
	/* ������������� �������� ����������� ���������������
	� combo box'� � ����������, �������������� ����������
	������������ ����� ����� � �������� */
	void _SetZoomInComboBox(DOUBLE dK);

	/* ������� scroll bar'� ������������� ���, ����� ���������������
	�������������� ������������ ������ ����. 
	���������� ����� ���������������*/
	void _MoveScrollBarsAfterZoom(CSize PreZoomSBSize, CSize PostZoomSBSize, 
		CPoint PreZoomSBPos);

	/* ���������� ������� scroll bar'�� �������������. 
	���� ������-���� scroll bar'� ���, �����. 0 ��� �����. 
	�������� CSize*/
	CSize _GetScrollBarSizes();

	// ���������� ������� ������� ���� ����� ����������
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:	// ����������� ���������
	// ���������� ������� ���������� ��������
	afx_msg void _OnZoomIn();

	// ���������� ������� ���������� ��������
	afx_msg void _OnZoomOut();

	// ���������� ������� �������������� ������������� ������� ����
	afx_msg void _OnRestoreZoom();

	// ���������� ������� ������������ ������� ���� �� ������� ����
	afx_msg void _OnFitZoom();

	// ���������� ������� ������ �������� �� combo box'� � ����������
	afx_msg void _OnCmbZoomSel();

	// ���������� ������� �������������� ���� ����� combo box'� � ����������
	afx_msg void _OnCmbZoomEditChange();

	// ������� �� ��������� � ����� �������
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	// ���������� ����� ����� ������ ���� �� �������������
	afx_msg void OnLButtonDown( UINT, CPoint);

	// ���������� ����� ������� ������ ���� (������� �� ������) �� �������������
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	
	/* ���������� ������� �������� ���� ��� ��������������
	������� scroll bar'� ����� �� �����, �������� "��������������" �����*/
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// ���������� ������� �������� ������ ����
	afx_msg BOOL OnMouseWheel(UINT nFlags, SHORT zDelta, CPoint pt);

	// ���������� ������� ���������� ������ ����
	afx_msg void _OnAddLayer();

	// ���������� ������� �������� �������� ����
	afx_msg void _OnDelLayer();

	// ���������� ������� ������ ����� ��������� ��� �������� ����
	afx_msg void _OnSelLayerColor();

	// ���������� ������� ����������� ���� ����� � ������� ���������
	afx_msg void _OnLayerUp();

	// ���������� ������� ����������� ���� ���� � ������� ���������
	afx_msg void _OnLayerDown();

	// ���������� ������� ������ ���� �� ListView'� �� ������
	afx_msg void _OnLstLayersClick(NMHDR *pNotifyStruct, LRESULT *pLRes);

	// ���������� ������� ��������� chek state � chek box'� "�������"
	afx_msg void _OnChkVisibleClick();

	/* ���������� �������� ���� ���-�������
	���� ������ �������� ����� - ������ ���,
	���� ��� - ����������*/
	afx_msg void _OnMnuZoom();

private: // ����
	// ���������� ������� � ������ ����� ������ ������ "�������������" �����
	CPoint m_pntStartCurPos;

	/* ������� ������� ��� ���� �����. 
	��� ��������������� ���������� ������ ���������� �� ��� �����*/
	DOUBLE m_dScale;

	// ��������������� �� combo box � ������� ��������
	BOOL bCmbInited;

private:	// ����� ��������
	DECLARE_MESSAGE_MAP()
};