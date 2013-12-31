#include "stdafx.h"
#include "GlobalData.h"
#include "ShapeViewer.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// --------------------------------------------------------

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// --------------------------------------------------------

CMainFrame::CMainFrame() {}

CMainFrame::~CMainFrame(){}

// --------------------------------------------------------

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1) return -1;

	if(!_InitLayersDlgBar()) return -1;
	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_LEFT, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndDlgBar))
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}
	
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	CComboBox *pcmbZoom = ((CComboBox*)m_wndDlgBar.GetDlgItem(IDC_CMB_ZOOM));
	pcmbZoom->SetCurSel(5);
	
	return 0;
}

// --------------------------------------------------------

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU | WS_THICKFRAME ;

	return TRUE;
}

// --------------------------------------------------------

BOOL CMainFrame::_InitLayersDlgBar() {
	if (!m_wndLayers.Create(this, IDD_LAYERS,
		CBRS_ALIGN_RIGHT, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return FALSE;
	}

	CListCtrl *plstLayers =(CListCtrl*) m_wndLayers.GetDlgItem(IDC_LST_LAYERS);
	CRect ListClRect;
	plstLayers->GetClientRect(&ListClRect);
	INT ColumnWidth = INT(ListClRect.Width() * 0.75);
	plstLayers->InsertColumn(0, "Название", LVCFMT_LEFT, ColumnWidth);
	plstLayers->InsertColumn(1, "Видим", LVCFMT_LEFT, ListClRect.Width() - ColumnWidth);
	return TRUE;
}