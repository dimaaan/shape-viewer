#pragma once

class CMainFrame : public CFrameWnd {
public:
	CMainFrame();
	virtual ~CMainFrame();

	CToolBar* GetToolBar() {return &m_wndToolBar;}

	CReBar* GetReBar() {return &m_wndReBar;}

	CDialogBar* GetZoomDlg() {return &m_wndDlgBar;}

	CDialogBar* GetLayersDlg() {return &m_wndLayers;}

private:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	BOOL _InitLayersDlgBar();

private:  // control bar embedded members
	CToolBar    m_wndToolBar;
	CReBar      m_wndReBar;
	CDialogBar  m_wndDlgBar;
	CDialogBar	m_wndLayers;

	DECLARE_DYNCREATE(CMainFrame)
	DECLARE_MESSAGE_MAP()
};