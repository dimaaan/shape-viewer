#include "stdafx.h"
#include "GlobalData.h"
#include "ShapeViewer.h"
#include "ShapeViewerDoc.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// --------------------------------------------------------

CShapeViewerDoc::CShapeViewerDoc(){}

CShapeViewerDoc::~CShapeViewerDoc() {
	DeleteContents();
}

// --------------------------------------------------------
// обработчик открытия нового документа
BOOL CShapeViewerDoc::OnNewDocument() {
	if (!CDocument::OnNewDocument())
		return FALSE;
	DeleteContents();
	return TRUE;
}

// --------------------------------------------------------
// процедура загрузки shape-файла
void CShapeViewerDoc::Serialize(CArchive& ar) {
	if(ar.IsLoading()) {
		SMap NewLayer;
		BOOL bAbortLoad = FALSE;
		if(!LoadFromFile(ar, NewLayer, &bAbortLoad)) return;
		if(bAbortLoad) return;
		m_Layers.push_front(NewLayer);

		// при необходимости расширяем Bounding Box, 
		// чтоб новая карта не вышла за его пределы
		CRect BBox1 = (CRect) m_BoundBox,
			  BBox2 = (CRect) NewLayer.m_BoundBox;
		m_BoundBox = CMyRect(BBox1 | BBox2);

		// добавляем карту в список слоев
		CListCtrl *plstLayers = GET_LST_LAYERS;
		CString str = ar.GetFile()->GetFileTitle();
		str = str.Left(str.ReverseFind('.'));
		plstLayers->InsertItem(0, str);
		plstLayers->SetItemText(0, 1, "Да");

		UpdateAllViews(NULL);
	}
}

// --------------------------------------------------------
// удаляет все содержимое документа
void CShapeViewerDoc::DeleteContents() {
	for(UINT i = 0; i < m_Layers.size(); i++)
		_DeleteShapes(&m_Layers[i]);
	m_BoundBox.SetRect(0,0,0,0);
	UpdateAllViews(NULL);
}

// --------------------------------------------------------
/* загружает данные из Shape файла в структуру SMap.
При удачной загрузке возвр. TRUE, при ошибке FALSE*/
BOOL CShapeViewerDoc::LoadFromFile(CArchive &ar, SMap &Layer, BOOL *pbAbortLoad) {
	CFile *pFile = ar.GetFile();
	ASSERT(pFile);

	// читаем и проверяем сигнатуру
	TRY {
		INT iSignature = _ReadBigIndianDWORD(*pFile);
		if(iSignature != SIGNATURE) {
			// неправильная сигнатура - некритичная ошибка
			CString ErrStr;
			ErrStr.Format(IDS_ERR_BAD_SIGNATURE2, pFile->GetFilePath());
			if(_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, FALSE))
				return FALSE;
		}
	}
	CATCH(CFileException, pEx) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_SIGNATURE, (LPCTSTR) pFile->GetFilePath());
		_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, TRUE);
		return FALSE;
	}
	END_CATCH;

	// читаем длинну файла
	pFile->Seek(24, CFile::begin);
	INT iFileLen = 0;
	TRY {
		iFileLen = _ReadBigIndianDWORD(*pFile);
	}
	CATCH(CFileException, pEx) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_FILE_LEN, (LPCTSTR) pFile->GetFilePath());
		_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, TRUE);
		return FALSE;
	}
	END_CATCH;
	iFileLen *= 2; // находим длинну файла в байтах (до этого она была в словах)
	if(iFileLen != pFile->GetLength()) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_FILE_LEN2, (LPCTSTR) pFile->GetFilePath());
		if(_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, FALSE))
			return FALSE;
	}

	// читаем версию файла
	INT iFileVersion = 0;
	TRY {
		iFileVersion = _ReadLittleIndianDWORD(*pFile);
	}
	CATCH(CFileException, pEx) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_VERSION, (LPCTSTR) pFile->GetFilePath());
		_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, TRUE);
		return FALSE;
	}
	END_CATCH;
	if(iFileVersion != FILE_FORMAT_VERSION) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_FILE_NEW_VERSION, (LPCTSTR) pFile->GetFilePath(), 
			iFileVersion, FILE_FORMAT_VERSION);
		if(_ReportError(ErrStr, IDS_ERR_BAD_VERSION, FALSE))
			return FALSE;
	}

	// читаем Shape Type
	TRY {
		Layer.m_iShapeType = _ReadLittleIndianDWORD(*pFile);
	}
	CATCH(CFileException, pEx) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_SHAPE_TYPE, (LPCTSTR) pFile->GetFilePath());
		_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, TRUE);
		return FALSE;
	}
	END_CATCH;
	if( Layer.m_iShapeType != ST_POINT &&
		Layer.m_iShapeType != ST_MULTIPOINT &&
		Layer.m_iShapeType != ST_ARC &&
		Layer.m_iShapeType != ST_POLYGON)
	{
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_SHAPE_TYPE2, Layer.m_iShapeType, pFile->GetFilePath());
		if(_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, FALSE))
			return FALSE;
	}

	// читаем Bounding Box
	TRY {
		Layer.m_BoundBox.SetXMin(_ReadLittleIndianDOUBLE(*pFile));
		Layer.m_BoundBox.SetYMin(_ReadLittleIndianDOUBLE(*pFile));
		Layer.m_BoundBox.SetXMax(_ReadLittleIndianDOUBLE(*pFile));
		Layer.m_BoundBox.SetYMax(_ReadLittleIndianDOUBLE(*pFile));
	}
	CATCH(CFileException, pEx) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_BOUNDBOX, pFile->GetFilePath());
		_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, TRUE);
		return FALSE;
	}
	END_CATCH;
	
	pFile->Seek(100, CFile::begin);
	CShape *pLoadedShape;
	do {
		pLoadedShape = _ReadShape(*pFile, Layer, pbAbortLoad);
		if(*pbAbortLoad) return FALSE;
		if(pLoadedShape != NULL)
			Layer.m_Shapes.push_back(pLoadedShape);
	} while (pLoadedShape != NULL && pFile->GetPosition() != iFileLen);

	// существует ли БД для этого файла
	CFileStatus fs;
	if(CFile::GetStatus(ar.m_strFileName, fs)) {
			
	}

	return TRUE;
}

// --------------------------------------------------------
/* читаем фигуру (заголвок и содержимое) с текущей позиции в файле
в структуру SMap.
Эта ф-ия вспомогательная для LoadFromFile
Текущая позиция файла должна быть установлена на начало записи */
CShape* CShapeViewerDoc::_ReadShape(CFile &file, SMap &Layer, BOOL *pbAbortLoad) {
	static INT iShapesCount = 0;
	CShape *res = NULL;

	// считываем номер фигуры
	INT iRecordNumber = 0;
	TRY {
		iRecordNumber = _ReadBigIndianDWORD(file);
	}
	CATCH(CFileException, pEx) {
		if(iShapesCount == 0) {
			CString ErrStr;
			ErrStr.LoadString(IDS_ERR_BAD_RECORD_NUMBER);
			_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, TRUE);
			return NULL;
		}
		else
			return NULL; // файл корректно прочитан до конца
	}
	END_CATCH;
	if(iRecordNumber < 1) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_RECORD_NUMBER2, iRecordNumber, file.GetPosition(), file.GetFilePath());
		if(_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, FALSE))
			return NULL;
	}

	// считываем длинну содержимого записи
	INT iContentLen = 0;
	TRY {
		iContentLen = _ReadBigIndianDWORD(file);
	}
	CATCH(CFileException, pEx) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_CONTENT_LEN, iRecordNumber, file.GetPosition(), file.GetFilePath());
		_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, TRUE);
		return NULL;
	}
	END_CATCH;

	switch(Layer.m_iShapeType) {
		case ST_POINT:
			res = _ReadDot(file, Layer, iRecordNumber, iContentLen, pbAbortLoad);
			break;
		case ST_MULTIPOINT:
			AfxMessageBox("Not implement", NULL, MB_OK);
			break;
		case ST_ARC:
			res = _ReadArcPoly(file, Layer, ST_ARC, iRecordNumber, iContentLen, pbAbortLoad);
			break;
		case ST_POLYGON:
			res = _ReadArcPoly(file, Layer, ST_POLYGON, iRecordNumber, iContentLen, pbAbortLoad);
			break;
		default:
			ASSERT(0);
	}

	if(*pbAbortLoad) return NULL;
	if(res != NULL) iShapesCount++;
	return res;
}

// --------------------------------------------------------
/* читает из текущей позиции открытого 
файла двойное слово в формате  Big-Indian 
Если достигнут конец файла порождает исключение CFileException*/
DWORD CShapeViewerDoc::_ReadBigIndianDWORD(CFile &f) {
	DWORD dwRes = 0;
	BYTE bBytes[4];
	
	if(f.Read(bBytes, 4) != 4)
		AfxThrowFileException(CFileException::endOfFile, -1, f.GetFilePath());
	
	dwRes = MAKELONG(MAKEWORD(bBytes[3], bBytes[2]), MAKEWORD(bBytes[1], bBytes[0]));
	return dwRes;
}

// --------------------------------------------------------
/* читает из текущей позиции открытого 
файла двойное слово в формате  Little-Indian 
Если достигнут конец файла порождает исключение CFileException*/
DWORD CShapeViewerDoc::_ReadLittleIndianDWORD(CFile &f) {
	DWORD dwRes = 0;
	BYTE bBytes[4];
	
	if(f.Read(bBytes, 4) != 4)
		AfxThrowFileException(CFileException::endOfFile, -1, f.GetFilePath());
	
	dwRes = MAKELONG(MAKEWORD(bBytes[0], bBytes[1]), MAKEWORD(bBytes[2], bBytes[3]));
	return dwRes;
}

// --------------------------------------------------------
/* читает из текущей позиции открытого 
файла в формате  Little-Indian переменную типа 64-bit IEEE double */
DOUBLE CShapeViewerDoc::_ReadLittleIndianDOUBLE(CFile &f) {
	DOUBLE dRes = 0;
	BYTE bBytes[sizeof(DOUBLE)];

	if(f.Read(bBytes, sizeof(DOUBLE)) != sizeof(DOUBLE))
		AfxThrowFileException(CFileException::endOfFile, -1, f.GetFilePath());
	CopyMemory(&dRes, bBytes, sizeof(dRes));
	return dRes;
}

// --------------------------------------------------------
/* читает из текущей позиции открытого файла
	содержимое записи c фигурой типа Point
При ошибке 
	1 возвращает NULL
	2 показывает диалог с ошибкой и выбором: прервать загрузку или нет
	2 Если пользоватеь выбрал прервать загрузку файла *pAbortLoad == TRUE,
	  если нет - FALSE*/
CDot* CShapeViewerDoc::_ReadDot(CFile &f, SMap &Layer, INT iRecordNumber, INT iContentLen, BOOL *pbAbortLoad, 
							   BOOL SkipShapeType /*= FALSE*/)
{
	if(iRecordNumber < 1 || iContentLen < 1) return NULL;

	if(!SkipShapeType) {
		INT ShapeType = _ReadShapeType(f, ST_POINT, iRecordNumber, iContentLen);
		if(ShapeType == -1) return NULL;
	}

	// считываем координаты точки
	DOUBLE dX = 0, dY = 0;
	TRY {
		dX = _ReadLittleIndianDOUBLE(f);
		dY = _ReadLittleIndianDOUBLE(f);
	}
	CATCH(CFileException, pEx) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_DOT_COORD, iRecordNumber, iContentLen, f.GetFilePath());
		*pbAbortLoad = _ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, FALSE);
		return NULL;
	}
	END_CATCH;

	if(!Layer.m_BoundBox.PointInRect(dX, dY)) {
		// координаты точки находятся за пределами bounding box.
		CString ErrStr;
		ErrStr.Format(IDS_ERR_DOT_OUT_OF_BOUNDBOX, dX, dY, iRecordNumber, 
			iContentLen, f.GetFilePath());
		*pbAbortLoad = _ReportError(ErrStr, IDS_ERR_GEOMETRY_ERROR, FALSE);
			return NULL;
	}

	CDot *pNewDot = new CDot(dX, dY);
	return pNewDot;
}

// --------------------------------------------------------
/* читает из текущей позиции открытого файла
	содержимое записи c фигурой типа Arc или Polygon,
	в зависимости от значения ShapeT
При ошибке 
	1 возвращает NULL
	2 показывает диалог с ошибкой и выбором: прервать загрузку или нет
	2 Если пользоватеь выбрал прервать загрузку файла *pAbortLoad == TRUE,
	  если нет - FALSE*/
CShape* CShapeViewerDoc::_ReadArcPoly(CFile &f, SMap &Layer, EShapeType ShapeT, 
						INT iRecordNumber, INT iContentLen, BOOL *pbAbortLoad)
{
	/* записи типа Arc и Polygon похожи, поэтому
	для их загрузки лучше использовать одну функцию*/
	if(iRecordNumber < 1 || iContentLen < 1) return NULL;

	INT iShapeType = _ReadShapeType(f, ShapeT, iRecordNumber, iContentLen);
	if(iShapeType == -1) return NULL;

	CMyRect BoundBox;
	if(!_ReadBoundingBox(f, iRecordNumber, iContentLen, Layer, &BoundBox)) return NULL;

	// читаем поле NumParts
	INT iNumParts = 0;
	TRY {
		iNumParts = _ReadLittleIndianDWORD(f);
	}
	CATCH(CFileException, pEx) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_ARC_NUM_PARTS, iRecordNumber, iContentLen,
			f.GetPosition(),f.GetFilePath());
		_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, TRUE);
		return NULL;
	}
	END_CATCH;

	if(iNumParts < 1) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_ARC_NUM_PARTS2, iRecordNumber, iNumParts, iContentLen,
			f.GetPosition(),f.GetFilePath());
		if(_ReportError(ErrStr, IDS_ERR_GEOMETRY_ERROR, FALSE))
			return NULL;
	}

	// читаем поле NumPoints
	INT iNumPoints = 0;
	TRY {
		iNumPoints = _ReadLittleIndianDWORD(f);
	}
	CATCH(CFileException, pEx) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_ARC_NUM_POINTS, iRecordNumber, iContentLen, 
			f.GetPosition(),f.GetFilePath());
		_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, TRUE);
		return NULL;
	}
	END_CATCH;

	if(iNumPoints < 1) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_ARC_NUM_POINTS2, iRecordNumber, iNumPoints, iContentLen,
			f.GetPosition(),f.GetFilePath());
		if(_ReportError(ErrStr, IDS_ERR_GEOMETRY_ERROR, FALSE))
			return NULL;
	}

	// читаем поле Parts
	INT i = 0;
	INT *ArrParts = new INT[iNumParts];
	TRY {
		for(i = 0; i < iNumParts; i++)
			ArrParts[i] = _ReadLittleIndianDWORD(f);
	}
	CATCH(CFileException, pEx) {
		CString ErrStr;
		ErrStr.Format(IDS_ERR_BAD_ARC_PARTS, iRecordNumber, iContentLen, 
			f.GetPosition(),f.GetFilePath());
		_ReportError(ErrStr, IDS_ERR_FILE_OPEN_ERROR, TRUE);
		return NULL;
	}
	END_CATCH;

	for(i = 0; i < iNumParts; i++) {
		if(ArrParts[i] < 0 || ArrParts[i] >= iNumPoints) {
			CString ErrStr;
			ErrStr.Format(IDS_ERR_BAD_ARC_NUM_PARTS2, ShapeNames[ST_ARC], iRecordNumber, 
				iNumPoints, iContentLen, f.GetPosition(),f.GetFilePath());
			if(_ReportError(ErrStr, IDS_ERR_GEOMETRY_ERROR, FALSE))
				return NULL;
			// если индекс вершины поврежден, даем ему любое допустимое значение.
			// при это фигура будет поврежденной, но все-таки немного сохранит свои очертания
			ArrParts[i] = i;
		}
	}

	// читаем поле Points
	CDot *ArrPoints = new CDot[iNumPoints], *pLoadedDot;

	for(i = 0; i < iNumPoints; i++) {
		pLoadedDot = _ReadDot(f, Layer, iRecordNumber, iContentLen, pbAbortLoad, TRUE);
		if(pLoadedDot == NULL) {
			if(pbAbortLoad) 
				return NULL;
			else {
				// если координаты точки повреждены, то задаем их нолями
				// и продолжаем работу
				ArrPoints[i] = CDot(0, 0);
				continue;
			}
		}
		else {
			ArrPoints[i] = *pLoadedDot;
		}
	}

	// все данные считаны, создаем по ним фигуру
	CShape *pNewShape;
	if(ShapeT == ST_ARC)
		pNewShape = new CArc(BoundBox, ArrParts, iNumParts, ArrPoints, iNumPoints);
	else if(ShapeT == ST_POLYGON)
		pNewShape = new CPolygon(BoundBox, ArrParts, iNumParts, ArrPoints, iNumPoints);
	else
		return NULL;

	delete[] ArrParts;
	delete[] ArrPoints;
	return pNewShape;
}

// --------------------------------------------------------
/*Вспомогательная ф-ия для ф-ий ReadИмяФигуры
	Читает поле Shape Type в содержимом текущей записи
	Если считанный тип записи неравен St сообщает об ошибке и возвр. -1
	При ошибке возвращает -1*/
INT CShapeViewerDoc::_ReadShapeType(CFile &f, EShapeType St, INT iRecordNumber, INT iContentLen) {
	if(iRecordNumber < 1 || iContentLen < 1) return NULL;

	INT iShapeType = -1;
	TRY {
		iShapeType = _ReadLittleIndianDWORD(f);
	}
	CATCH(CFileException, pEx) {
		CString ErrStr, Caption;
		ErrStr.Format(IDS_ERR_BAD_SHAPE_T, ShapeNames[St], iRecordNumber, iContentLen, f.GetFilePath());
		Caption.LoadString(IDS_ERR_FILE_OPEN_ERROR);
		MessageBox(theApp.m_pMainWnd->m_hWnd, ErrStr, Caption, MB_OK | MB_ICONWARNING);
		return -1;
	}
	END_CATCH;

	if(iShapeType != St) {
		CString ErrStr, Caption;
		ErrStr.Format(IDS_ERR_BAD_SHAPE_T2, iShapeType, ShapeNames[iShapeType], 
			St, ShapeNames[St], iRecordNumber, iContentLen, f.GetFilePath());
		Caption.LoadString(IDS_ERR_FILE_OPEN_ERROR);
		MessageBox(theApp.m_pMainWnd->m_hWnd, ErrStr, Caption, MB_OK | MB_ICONWARNING);
		return -1;
	}
	return iShapeType;
}

// --------------------------------------------------------
/*Вспомогательная ф-ия для ф-ий ReadИмяФигуры
	Читает поле Bounding Box в содержимом текущей записи фигуры
	При ошибке возвращает FALSE, иначе - TRUE
	Если фигура находится вне Bounding Box'а карты выводит
	предупредительное сообщение*/
BOOL CShapeViewerDoc::_ReadBoundingBox(CFile &f, INT iRecordNumber,
								INT iContentLen, SMap &Layer, CMyRect *pBBox) 
{
	if(iRecordNumber < 1 || iContentLen < 1) return FALSE;
	ASSERT(pBBox);

	TRY {
		pBBox->SetXMin(_ReadLittleIndianDOUBLE(f));
		pBBox->SetYMin(_ReadLittleIndianDOUBLE(f));
		pBBox->SetXMax(_ReadLittleIndianDOUBLE(f));
		pBBox->SetYMax(_ReadLittleIndianDOUBLE(f));
	}
	CATCH(CFileException, pEx) {
		CString ErrStr, Caption;
		ErrStr.Format(IDS_ERR_BAD_BOUNDBOX2, iRecordNumber, iContentLen, f.GetFilePath());
		Caption.LoadString(IDS_ERR_FILE_OPEN_ERROR);
		MessageBox(theApp.m_pMainWnd->m_hWnd, ErrStr, Caption, MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	END_CATCH;

	if(!pBBox->InRect(Layer.m_BoundBox)) {
		CString ErrStr, Caption;
		ErrStr.Format(IDS_ERR_SHAPE_BBOX_OUT_OF_BBOX, iRecordNumber, iContentLen, f.GetFilePath());
		Caption.LoadString(IDS_ERR_GEOMETRY_ERROR);
		MessageBox(theApp.m_pMainWnd->m_hWnd, ErrStr, Caption, MB_OK | MB_ICONWARNING);
	}
	return TRUE;
}

// --------------------------------------------------------
/* Выводит на экран сообщение об ощибке в загрузке файла. 
	Если ошибка критическая(IsCritical == TRUE), то 
	просто показывает сообщение с кнопкой OK и всегда возвр. FALSE
	Если ошика не критичкая (IsCritical == FALSE), 
	то показывает кнопки Yes и No и в конце сообщения приписывает
	"Прервать ли загрузку или нет?"
	Если пользователь выбирает Yes возвр. TRUE, No - FALSE*/
BOOL CShapeViewerDoc::_ReportError(CString ErrMsg, UINT CaptionID, 
								  BOOL IsCritical /*= FALSE*/) 
{
	CString Caption;
	Caption.LoadString(CaptionID);
	if(IsCritical == FALSE) {
		ErrMsg += "\n\nПрервать загрузку файла?";
		INT Res = MessageBox(theApp.m_pMainWnd->m_hWnd, ErrMsg, Caption, 
			MB_YESNO | MB_ICONWARNING | MB_DEFBUTTON1);
		return Res == IDYES ? TRUE : FALSE;
	}
	else {
		MessageBox(theApp.m_pMainWnd->m_hWnd, ErrMsg, Caption, MB_OK | MB_ICONWARNING);
		return FALSE;
	}
}

// --------------------------------------------------------
/*Очищает карту, корректно освобождая память из-под
	диманически выделенных объектов*/
void CShapeViewerDoc::_DeleteShapes(SMap *pLayer) {
	for(UINT i = 0; i < pLayer->m_Shapes.size(); i++)
		delete pLayer->m_Shapes[i];
	pLayer->m_Shapes.clear();
}

// --------------------------------------------------------

void CShapeViewerDoc::_ConnectToDB(CFile &f, SMap *pLayer) {
	/*CString DBPath;
	DBPath = f.GetFilePath() + f.GetFileTitle() + '.' + DATABASE_EXTENSION;

	CString ConnStr;
	ConnStr = "DNS=dBASE Files UID=ShapeViewer DBQ=" + DBPath;
	
	pLayer->m_DB.OpenEx(ConnStr, CDatabase::openReadOnly);*/
}

// --------------------------------------------------------

IMPLEMENT_DYNCREATE(CShapeViewerDoc, CDocument)

BEGIN_MESSAGE_MAP(CShapeViewerDoc, CDocument)
END_MESSAGE_MAP()