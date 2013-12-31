#include "stdafx.h"
#include "GlobalData.h"
#include "ShapeViewer.h"
#include "ShapeViewerDoc.h"
#include "ShapeViewerView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning (disable:4244) //conversion from 'DOUBLE' to 'LONG', possible loss of data

IMPLEMENT_DYNCREATE(CShapeViewerView, CScrollView)

// --------------------------------------------------------

BEGIN_MESSAGE_MAP(CShapeViewerView, CScrollView)
	ON_COMMAND(ID_FILE_PRINT,			CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT,	CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW,	CScrollView::OnFilePrintPreview)
	ON_COMMAND(ID_ZOOM_IN,				CShapeViewerView::_OnZoomIn)
	ON_COMMAND(ID_ZOOM_OUT,				CShapeViewerView::_OnZoomOut)
	ON_COMMAND(ID_RESTORE_SCALE,		CShapeViewerView::_OnRestoreZoom)
	ON_COMMAND(ID_FIT_SCALE,			CShapeViewerView::_OnFitZoom)
	ON_COMMAND(ID_ADD_LAYER,			CShapeViewerView::_OnAddLayer)
	ON_COMMAND(IDM_ZOOM,				CShapeViewerView::_OnMnuZoom)
	
	ON_COMMAND(IDC_BTN_ADD_LAYER,		CShapeViewerView::_OnAddLayer)
	ON_COMMAND(IDC_BTN_DEL_LAYER,		CShapeViewerView::_OnDelLayer)
	ON_COMMAND(IDC_BTN_SEL_COLOR,		CShapeViewerView::_OnSelLayerColor)
	ON_COMMAND(IDC_BTN_UP,				CShapeViewerView::_OnLayerUp)
	ON_COMMAND(IDC_BTN_DOWN,			CShapeViewerView::_OnLayerDown)

	ON_NOTIFY(NM_CLICK, IDC_LST_LAYERS, CShapeViewerView::_OnLstLayersClick)

	ON_BN_CLICKED(IDC_BTN_ZOOM_IN,		CShapeViewerView::_OnZoomIn)
	ON_BN_CLICKED(IDC_BTN_ZOOM_OUT,		CShapeViewerView::_OnZoomOut)
	ON_BN_CLICKED(IDC_BTN_RESTORE_ZOOM, CShapeViewerView::_OnRestoreZoom)
	ON_BN_CLICKED(IDC_BTN_FIT_ZOOM,		CShapeViewerView::_OnFitZoom)
	ON_BN_CLICKED(IDC_CHK_VISIBLE,		CShapeViewerView::_OnChkVisibleClick)

	ON_CBN_SELENDOK(IDC_CMB_ZOOM,		CShapeViewerView::_OnCmbZoomSel)
	ON_CBN_EDITCHANGE(IDC_CMB_ZOOM,		CShapeViewerView::_OnCmbZoomEditChange)

	ON_WM_LBUTTONDOWN()
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()

	ON_WM_ERASEBKGND()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

// --------------------------------------------------------

CShapeViewerView::CShapeViewerView() {
	SetScrollSizes(MM_TEXT, CSize(1,1));
	m_dScale = 1.0f;
	bCmbInited = FALSE;
	srand((UINT) time(NULL));
}

CShapeViewerView::~CShapeViewerView(){}

// --------------------------------------------------------
// отрисовка окна
void CShapeViewerView::OnDraw(CDC* pDC) {
	bCmbInited = TRUE;
	CShapeViewerDoc* pDoc = GetDocument();
	/*
	CRect ClRect;
	GetClientRect(&ClRect);
	pDC->FillSolidRect(&ClRect, RGB(255,255,255));
	CDC BackBuf;
	CBitmap bmp;
	DOUBLE dMapWidth = pDoc->m_BoundBox.GetWidth() * m_dScale,
		dMapHeight = pDoc->m_BoundBox.GetHeight() * m_dScale;
	bmp.CreateCompatibleBitmap(pDC, dMapWidth, dMapHeight);
	BackBuf.CreateCompatibleDC(pDC);
	BackBuf.SelectObject(&bmp);*/

	INT iXMin = Round(pDoc->GetBoundingBox().GetXMin() * m_dScale),
		iYMin = Round(pDoc->GetBoundingBox().GetYMin() * m_dScale);

	//BackBuf.FillSolidRect(CRect(0,0,dMapWidth, dMapHeight), RGB(255,255,255));

	pDC->SetMapMode(MM_ANISOTROPIC);
	// устанавливаем начало координат
	pDC->SetWindowOrg(iXMin, iYMin);

	/*BackBuf.SetMapMode(MM_ANISOTROPIC);
	// устанавливаем начало координат
	BackBuf.SetWindowOrg(iXMin, iYMin);*/

	// рисуем все слои
	CPen MapPen;
	COLORREF CurrCol;
	for(UINT i = 0; i < pDoc->GetLayers()->size(); i++) {
		if(!pDoc->GetLayers()->at(i).m_bVisible) continue;
		CurrCol = pDoc->GetLayers()->at(i).m_Color;
		MapPen.CreatePen(PS_SOLID, 1, CurrCol);
		pDC->SelectObject(&MapPen);
		for(UINT j = 0; j < pDoc->GetLayers()->at(i).m_Shapes.size(); j++) {
			pDoc->GetLayers()->at(i).m_Shapes.at(j)->Draw(*pDC, m_dScale, CurrCol);
		}
		MapPen.DeleteObject();
	}

	//pDC->BitBlt(iXMin, iYMin, dMapWidth, dMapHeight, &BackBuf, iXMin, iYMin, SRCCOPY);
}

// --------------------------------------------------------
// обработчик события заливки окна перед отрисовкой
BOOL CShapeViewerView::OnEraseBkgnd(CDC* pDC ) {
	return CScrollView::OnEraseBkgnd(pDC);
}

// --------------------------------------------------------

inline CShapeViewerDoc* CShapeViewerView::GetDocument() const {
	return reinterpret_cast<CShapeViewerDoc*>(m_pDocument);
}

// --------------------------------------------------------
// реакция на сообщение о смене курсора
BOOL CShapeViewerView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	::SetCursor(theApp.LoadStandardCursor(IDC_SIZEALL));
	return TRUE;
}

// --------------------------------------------------------
// обработчик события увеличения масштаба
void CShapeViewerView::_OnZoomIn() {
	CSize OldSBSize(_GetScrollBarSizes());
	m_dScale *= 1 + ZOOM_STEP;
	UpdateScrollBars();
	CSize NewSBSize(_GetScrollBarSizes());
	_MoveScrollBarsAfterZoom(OldSBSize, NewSBSize, GetScrollPosition());
	Invalidate();
	_SetZoomInComboBox(m_dScale);
}

// --------------------------------------------------------
// обработчик события уменьшения масштаба
void CShapeViewerView::_OnZoomOut() {
	CSize OldSBSize(_GetScrollBarSizes());
	m_dScale *= 1 - ZOOM_STEP;
	if(m_dScale <= 0) m_dScale = 0;
	UpdateScrollBars();
	CSize NewSBSize(_GetScrollBarSizes());
	_MoveScrollBarsAfterZoom(OldSBSize, NewSBSize, GetScrollPosition());
	Invalidate();
	_SetZoomInComboBox(m_dScale);
}

// --------------------------------------------------------
// обработчик события восстановления оригинального размера карт
void CShapeViewerView::_OnRestoreZoom() {
	CSize OldSBSize(_GetScrollBarSizes());
	m_dScale = 1.0f;
	Invalidate();
	_SetZoomInComboBox(m_dScale);
	UpdateScrollBars();
	CSize NewSBSize(_GetScrollBarSizes());
	_MoveScrollBarsAfterZoom(OldSBSize, NewSBSize, GetScrollPosition());
}

// --------------------------------------------------------
// обработчик события выравнивания размера карт по размеру окна
void CShapeViewerView::_OnFitZoom() {
	CShapeViewerDoc* pDoc = GetDocument();

	CRect ClientRect;
	GetClientRect(&ClientRect);

	if(ClientRect.Width() < ClientRect.Height()) 
		m_dScale = ClientRect.Width() / pDoc->GetBoundingBox().GetWidth();
	else 
		m_dScale = ClientRect.Height() / pDoc->GetBoundingBox().GetHeight();

	Invalidate();
	_SetZoomInComboBox(m_dScale);
	UpdateScrollBars();
}

// --------------------------------------------------------
// обработчик события выбора масштаба из combo box'а с масштабами
void CShapeViewerView::_OnCmbZoomSel() {
	CComboBox *pcmbZoom = GET_CMB_ZOOM;
	CString str;
	pcmbZoom->GetLBText(pcmbZoom->GetCurSel(), str);
	DOUBLE dVal = atof((LPCTSTR) str);

	CSize OldSBSize(_GetScrollBarSizes());
	m_dScale = dVal / 100;
	UpdateScrollBars();
	CSize NewSBSize(_GetScrollBarSizes());
	_MoveScrollBarsAfterZoom(OldSBSize, NewSBSize, GetScrollPosition());
	Invalidate();
	_SetZoomInComboBox(m_dScale);
}

// --------------------------------------------------------
// обработчик события редактирования поля ввода combo box'а с масштабами
void CShapeViewerView::_OnCmbZoomEditChange() {
	CComboBox *pcmbZoom = GET_CMB_ZOOM;
	CString str;
	pcmbZoom->GetWindowText(str);
	DOUBLE dVal = atof((LPCTSTR) str);

	CSize OldSBSize(_GetScrollBarSizes());
	m_dScale = dVal / 100;
	UpdateScrollBars();
	CSize NewSBSize(_GetScrollBarSizes());
	_MoveScrollBarsAfterZoom(OldSBSize, NewSBSize, GetScrollPosition());
	Invalidate();
	_SetZoomInComboBox(m_dScale);
}

// --------------------------------------------------------
/* вызывается когда документ был модифицирован,
с помощью CDocument::UpdateAllViews и нужна, чтоб отобразить 
все изменения документа на экране */
void CShapeViewerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) {
	UpdateScrollBars();
	// если это 1ый слой (или после удаления остался только 1 слой)
	// подгоним его размеры под размер окна
	CShapeViewerDoc* pDoc = GetDocument();
	if(pDoc->GetLayers()->size() == 1) _OnFitZoom();
}

// --------------------------------------------------------
// обработчик клика левой кнопки мыши на представлении
void CShapeViewerView::OnLButtonDown(UINT nFalgs, CPoint point) {
	GetCursorPos(&m_pntStartCurPos);
	ScreenToClient(&m_pntStartCurPos);
}

// --------------------------------------------------------
// обработчик клика средней кнопки мыши (нажатие на колесо) на представлении
void CShapeViewerView::OnMButtonDown(UINT nFlags, CPoint point) {
	OnLButtonDown(nFlags, point);
}

// --------------------------------------------------------
/* обработчик события движения мыши над представлением
Двигает scroll bar'ы вслед за мышью, реализуя "перетаскивание" мышью*/
void CShapeViewerView::OnMouseMove(UINT nFlags, CPoint point) {
	// если левая кнопка мыши не нажата - выходим
	if(!(nFlags & MK_LBUTTON) && !(nFlags & MK_MBUTTON)) return;
	
	INT iDx = GetScrollPos(SB_HORZ) - (m_pntStartCurPos.x - point.x) / 4,
		iDy = GetScrollPos(SB_VERT) - (m_pntStartCurPos.y - point.y) / 4;

	INT iMin = 0, iMax = 0;

	GetScrollRange(SB_HORZ, &iMin, &iMax);
	BOOL HasHorzScroll = !(iMin == 0 && iMax == 0);
	if(HasHorzScroll) SetScrollPos(SB_HORZ, iDx, TRUE);

	GetScrollRange(SB_VERT, &iMin, &iMax);
	BOOL HasVertScroll = !(iMin == 0 && iMax == 0);
	if(HasVertScroll) SetScrollPos(SB_VERT, iDy, TRUE);
	if(HasHorzScroll || HasVertScroll) RedrawWindow();
}

// --------------------------------------------------------
// обработчик события поворота колеса мыши
BOOL CShapeViewerView::OnMouseWheel(UINT nFlags, SHORT zDelta, CPoint pt) {
	if(zDelta > 0)
		_OnZoomIn();
	else 
		_OnZoomOut();
	return TRUE;
}

// --------------------------------------------------------
/* устанавливает заданный коэффициент масштабирования 
	в combo box'е с масштабами, предварительно преобразуя
	вещественное число дробь в проценты */
void CShapeViewerView::_SetZoomInComboBox(DOUBLE dK) {
	if(!bCmbInited) return;
	dK *= 100;
	CComboBox *pcmbZoom = GET_CMB_ZOOM;
	if(pcmbZoom == NULL) return;
	CString str;
	str.Format("%i", (INT) dK);
	str += "%";
	if(pcmbZoom->SelectString(0, str) == CB_ERR) {
		pcmbZoom->SetWindowText(str);
	}
}

// --------------------------------------------------------
/* двигает scroll bar'ы представления так, чтобы масштабирование
осуществлялось относительно центра окна. 
Вызывается после масштабирования*/
void CShapeViewerView::_MoveScrollBarsAfterZoom
		(CSize PreZoomSBSize, CSize PostZoomSBSize, CPoint PreZoomSBPos)
{
	CPoint NewSBPos(0, 0);
	if(PostZoomSBSize.cx != 0) {
		NewSBPos.x = PreZoomSBPos.x + (PostZoomSBSize.cx - PreZoomSBSize.cx) / 2;
	}
	if(PostZoomSBSize.cy != 0) {
		NewSBPos.y = PreZoomSBPos.y + (PostZoomSBSize.cy - PreZoomSBSize.cy) / 2;
	}
	SetScrollPos(SB_HORZ, NewSBPos.x);
	SetScrollPos(SB_VERT, NewSBPos.y);
}

// --------------------------------------------------------
/* возвращает размеры scroll bar'ов представления. 
	Если какого-либо scroll bar'а нет, возвр. 0 для соотв. 
	значения CSize*/
CSize CShapeViewerView::_GetScrollBarSizes() {
	INT iMinPos = 0, iMaxPos = 0;
	GetScrollRange(SB_HORZ, &iMinPos, &iMaxPos);
	INT iHorzSize = iMaxPos - iMinPos;
	GetScrollRange(SB_VERT, &iMinPos, &iMaxPos);
	INT iVertSize = iMaxPos - iMinPos;
	return CSize(iHorzSize, iVertSize);
}

// --------------------------------------------------------
/* задает размер scroll bar'а в соответствии с
	размером карты и масштабом */
void CShapeViewerView::UpdateScrollBars() {
	CShapeViewerDoc* pDoc = GetDocument();
	CSize s;
	s.cx = (LONG) (pDoc->GetBoundingBox().GetWidth() * m_dScale);
	s.cy = (LONG) (pDoc->GetBoundingBox().GetHeight() * m_dScale);
	SetScrollSizes(MM_TEXT, s);
}

// --------------------------------------------------------
// обработчик события добавления нового слоя
void CShapeViewerView::_OnAddLayer() {
	CShapeViewerDoc* pDoc = GetDocument();

	CFileDialog dlg(TRUE, EXTENSION, NULL, 
		OFN_ALLOWMULTISELECT | OFN_ENABLESIZING | OFN_EXPLORER |
		OFN_EXTENSIONDIFFERENT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		"Shape-файлы (*.shp)|*.shp|Все файлы (*.*)|*.*||", this);
	if(dlg.DoModal() != IDOK) return;

	POSITION Pos = dlg.GetStartPosition();
	CString strCurrFileName;
	CFile File;
	while(Pos != NULL) {
		strCurrFileName = dlg.GetNextPathName(Pos);
		File.Open(strCurrFileName, CFile::modeRead | CFile::shareExclusive);
		CArchive ar(&File, CArchive::load);
		pDoc->Serialize(ar);
		File.Close();
		ar.Close();
	}
}

// --------------------------------------------------------
// обработчик события удаления текущего слоя
void CShapeViewerView::_OnDelLayer() {
	CListCtrl *plstLayers = GET_LST_LAYERS;
	if(plstLayers->GetSelectedCount() == 0) return;
	INT iSelItem = plstLayers->GetNextItem(-1, LVNI_SELECTED);
	ASSERT(iSelItem != -1);
	// выводим диалог подтверждения на удаление
	CString strMsg;
	strMsg.LoadString(IDS_WARN_DELETE_LAYER);
	if(MessageBox(strMsg, NULL, MB_YESNO | MB_ICONQUESTION) == IDNO) return;
	// удаляем слой
	CShapeViewerDoc* pDoc = GetDocument();
	pDoc->GetLayers()->erase(pDoc->GetLayers()->begin() + iSelItem);
	plstLayers->DeleteItem(iSelItem);
	if(pDoc->GetLayers()->empty())
		pDoc->DeleteContents();
	Invalidate();
}

// --------------------------------------------------------
// обработчик события выбора цвета отрисовки для текущего слоя
void CShapeViewerView::_OnSelLayerColor() {
	// если ни один слой не активен - выходим
	CListCtrl *plstLayers = GET_LST_LAYERS;
	if(plstLayers->GetSelectedCount() == 0) return;
	
	// показываем диалог выбора цвета
	CColorDialog dlg;
	if(dlg.DoModal() != IDOK) return;
	
	// находим индекс выбранного слоя
	INT iSelItem = plstLayers->GetNextItem(-1, LVNI_SELECTED);
	ASSERT(iSelItem != -1);

	// устанавливаем выбранный цвет и перерисовываем окно
	CShapeViewerDoc* pDoc = GetDocument();
	pDoc->GetLayers()->at(iSelItem).m_Color = dlg.GetColor();
	Invalidate();
}

// --------------------------------------------------------
// обработчик события перемещения слоя вверх в порядке отрисовки
void CShapeViewerView::_OnLayerUp() {
	// если ни один слой не активен - выходим
	CListCtrl *plstLayers = GET_LST_LAYERS;
	if(plstLayers->GetSelectedCount() == 0) return;

	// находим индекс выбранного слоя
	INT iSelItem = plstLayers->GetNextItem(-1, LVNI_SELECTED);
	ASSERT(iSelItem != -1);

	//если слой и так самый первый - выходим
	if(iSelItem <= 0) return;

	// перемещаем слой в очереди слоев
	CShapeViewerDoc* pDoc = GetDocument();
	SMap Layer = pDoc->GetLayers()->at(iSelItem);
	pDoc->GetLayers()->erase(pDoc->GetLayers()->begin() + iSelItem);
	pDoc->GetLayers()->insert(pDoc->GetLayers()->begin() + iSelItem, Layer);
	
	// перемещаем слой в списке слоев (в list view'е) и перерисовываем окно
	CString strItemTxt, strSubItemTxt;
	strItemTxt = plstLayers->GetItemText(iSelItem, 0);
	strSubItemTxt = plstLayers->GetItemText(iSelItem, 1);
	plstLayers->DeleteItem(iSelItem);
	plstLayers->InsertItem(iSelItem - 1, strItemTxt);
	plstLayers->SetItemText(iSelItem - 1, 1, strSubItemTxt);
	Invalidate();
}

// --------------------------------------------------------
// обработчик события перемещения слоя вниз в порядке отрисовки
void CShapeViewerView::_OnLayerDown() {
	// если ни один слой не активен - выходим
	CListCtrl *plstLayers = GET_LST_LAYERS;
	if(plstLayers->GetSelectedCount() == 0) return;

	// находим индекс выбранного слоя
	INT iSelItem = plstLayers->GetNextItem(-1, LVNI_SELECTED);
	ASSERT(iSelItem != -1);

	//если слой и так самый последний - выходим
	if(iSelItem >= plstLayers->GetItemCount()) return;

	// перемещаем слой в очереди слоев
	CShapeViewerDoc* pDoc = GetDocument();
	SMap Layer = pDoc->GetLayers()->at(iSelItem);
	pDoc->GetLayers()->erase(pDoc->GetLayers()->begin() + iSelItem);
	pDoc->GetLayers()->insert(pDoc->GetLayers()->begin() + iSelItem - 1, Layer);

	// перемещаем слой в списке слоев (в list view'е) и перерисовываем окно
	CString strItemTxt, strSubItemTxt;
	strItemTxt = plstLayers->GetItemText(iSelItem, 0);
	strSubItemTxt = plstLayers->GetItemText(iSelItem, 1);
	plstLayers->DeleteItem(iSelItem);
	plstLayers->InsertItem(iSelItem + 1, strItemTxt);
	plstLayers->SetItemText(iSelItem + 1, 1, strSubItemTxt);
	Invalidate();
}

// --------------------------------------------------------
// обработчик события выбора слоя из ListView'а со слоями
void CShapeViewerView::_OnLstLayersClick(NMHDR *pNotifyStruct, LRESULT *pLRes) {
	CListCtrl *plstLayers = GET_LST_LAYERS;
	CButton *pchkVisible = (CButton*) GET_WND_LAYERS_ITEM(IDC_CHK_VISIBLE);
	// если слой не выбран, выходим
	if(plstLayers->GetSelectedCount() == 0) {
		pchkVisible->SetCheck(BST_UNCHECKED);
		pchkVisible->EnableWindow(FALSE);
		return;
	}
	// находим индекс выбранного слоя
	INT iSelItem = plstLayers->GetNextItem(-1, LVNI_SELECTED);
	ASSERT(iSelItem != -1);

	// получаем указатель на документ
	CShapeViewerDoc* pDoc = GetDocument();

	// устанавливаем  check box "Видимость" в соотв. положение
	if(pDoc->GetLayers()->at(iSelItem).m_bVisible)
		pchkVisible->SetCheck(BST_CHECKED);
	else
		pchkVisible->SetCheck(BST_UNCHECKED);
}

// --------------------------------------------------------
// обработчик события изменения chek state у chek box'а "Видимый"
void CShapeViewerView::_OnChkVisibleClick() {
	// если ни один слой не активен - выходим
	CListCtrl *plstLayers = GET_LST_LAYERS;
	if(plstLayers->GetSelectedCount() == 0) return;
	// получаем индекс активного слоя
	INT iSelItem = plstLayers->GetNextItem(-1, LVNI_SELECTED);
	ASSERT(iSelItem != -1);
	// получаем указатель на документ
	CShapeViewerDoc* pDoc = GetDocument();
	// устанавливаем поле bVisible у активного слоя в соотв. положение
	CButton *pchkVisible = (CButton*) GET_WND_LAYERS_ITEM(IDC_CHK_VISIBLE);
	BOOL bVisChkState = pchkVisible->GetCheck() == BST_CHECKED ? TRUE : FALSE;
	pDoc->GetLayers()->at(iSelItem).m_bVisible = bVisChkState;
	// устанавливаем правильное значение столбца "Видим" в списке слоев
	plstLayers->SetItemText(iSelItem, 1,
		bVisChkState == TRUE ? LAYER_VISIBLE : LAYER_INVISIBLE);
	Invalidate();
}

// --------------------------------------------------------
/* обработчик элемента Меню Вид-Масштаб
Если диалог масштаба виден - прячет его,
если нет - показывает*/
void CShapeViewerView::_OnMnuZoom() {
	CMenu *pMenu = GET_FRAME_WND->GetMenu();
	if(pMenu == NULL) return;

	if(pMenu->GetMenuState(IDM_ZOOM, MF_BYCOMMAND) & MF_CHECKED) {
		GET_FRAME_WND->GetReBar()->GetReBarCtrl().ShowBand(1, FALSE);
		pMenu->CheckMenuItem(IDM_ZOOM, MF_BYCOMMAND | MF_UNCHECKED);
	}
	else {
		GET_FRAME_WND->GetReBar()->GetReBarCtrl().ShowBand(1, TRUE);
		pMenu->CheckMenuItem(IDM_ZOOM, MF_BYCOMMAND | MF_CHECKED);
	}
}