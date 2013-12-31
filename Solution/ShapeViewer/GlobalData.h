#pragma once

/* �������� �� ������� �������������/����������� ������� ��
�������� "���������"/"���������" */
#define ZOOM_STEP				0.1;

// ��� �������� � ������� "�����" ListView'� �� ������
#define LAYER_VISIBLE			_T("��")
#define LAYER_INVISIBLE			_T("���")

#define SIGNATURE				9994	 // ��������� Shape-�����
#define FILE_FORMAT_VERSION		1000	 // ������ shape-������ � �������� �������� ���������
#define EXTENSION				_T("shp")// ���������� shape-������
#define DATABASE_EXTENSION		_T("dbf")// ���������� ����� ���� ������

// --------------------------------------------------------

// �������
#define GET_FRAME_WND	((CMainFrame*) theApp.m_pMainWnd)
#define GET_WND_LAYERS_ITEM(x) (GET_FRAME_WND->GetLayersDlg()->GetDlgItem(x))
#define GET_LST_LAYERS	((CListCtrl*) GET_WND_LAYERS_ITEM(IDC_LST_LAYERS))
#define GET_WND_ZOOM	(GET_FRAME_WND->GetZoomDlg())
#define GET_CMB_ZOOM	((CComboBox*) GET_WND_ZOOM->GetDlgItem(IDC_CMB_ZOOM))

// --------------------------------------------------------

// ��������� DOUBLE � ������������ ��� � INT
INT Round(DOUBLE x);