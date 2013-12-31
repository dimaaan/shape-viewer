#pragma once

/* величина на которую увеличивается/уменьшается масштаб по
комманде "Увеличить"/"Уменьшить" */
#define ZOOM_STEP				0.1;

// что показано в столбце "Видим" ListView'а со слоями
#define LAYER_VISIBLE			_T("Да")
#define LAYER_INVISIBLE			_T("Нет")

#define SIGNATURE				9994	 // сигнатура Shape-файла
#define FILE_FORMAT_VERSION		1000	 // версия shape-файлов с которыми работает программа
#define EXTENSION				_T("shp")// расширение shape-файлов
#define DATABASE_EXTENSION		_T("dbf")// расширение файла базы данных

// --------------------------------------------------------

// макросы
#define GET_FRAME_WND	((CMainFrame*) theApp.m_pMainWnd)
#define GET_WND_LAYERS_ITEM(x) (GET_FRAME_WND->GetLayersDlg()->GetDlgItem(x))
#define GET_LST_LAYERS	((CListCtrl*) GET_WND_LAYERS_ITEM(IDC_LST_LAYERS))
#define GET_WND_ZOOM	(GET_FRAME_WND->GetZoomDlg())
#define GET_CMB_ZOOM	((CComboBox*) GET_WND_ZOOM->GetDlgItem(IDC_CMB_ZOOM))

// --------------------------------------------------------

// округляет DOUBLE и конвертирует его в INT
INT Round(DOUBLE x);