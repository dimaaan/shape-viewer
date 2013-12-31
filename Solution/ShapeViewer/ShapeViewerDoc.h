#pragma once

#include <afxdb.h>          // MFC database support
#include <deque>
#include "Shape.h"

using namespace std;

/* Географическая карта. Состоит из очереди указателей на 
объекты одного и того же класа. 
Карта - логический элемент, с который оперирует пользователь.
Пользователь видит карту в виде слоя */
struct SMap {
	SMap() {
		m_Color = RGB(rand() % 200, rand() % 200, rand() % 200);
		m_bVisible = TRUE;
		m_IsDBUsed = FALSE;
	}

	~SMap() {
		//if(m_DB.IsOpen()) m_DB.Close();
	}

	/* очередь из указателей на объекты, наследованные от
	базового абстрактного CShape.
	Здесь храниться вся геометрия карты.*/
	deque<CShape*> m_Shapes;

	// фигуры какого типа содержит карта
	INT m_iShapeType;

	// цвет, которым надо рисовать карту на экране
	COLORREF m_Color;

	/* Определят, надо ли отрисовывать карту на экране или нет.
	Это нужно из расчета на то, что программа может отображать сразу
	несколько карт в виде слоев.*/
	BOOL m_bVisible;

	// Ограничивающий прямоугольник
	CMyRect m_BoundBox;

	/* показывает, используется ли вместе с shape-афйлом база данных,
	чтобы показывать пользователю дополнительную информацию.
	БД используется тогда, когда в папке с открываемым shp-файлом 
	есть bdf-файл с таким же именем*/
	BOOL m_IsDBUsed;

	/* База данных, с которой связная карта.
	Если m_IsDBUsed == TRUE, приложение соединено с БД,
	если FALSE, то БД для данной карты нет и с этим полем работать нельзя*/
	//CDatabase m_DB;
};

// --------------------------------------------------------------
/*Класс документа. Содержит все даныые по всем открытым картам,
отвечает за загрузку shape-файлов.*/
class CShapeViewerDoc : public CDocument {
public:
	virtual ~CShapeViewerDoc();

	deque<SMap>*	GetLayers() {return &m_Layers;}

	CMyRect&		GetBoundingBox() {return m_BoundBox;}
	void			SetBoundingBox(const CMyRect &NewBBox) {m_BoundBox = NewBBox;}

	// обработчик открытия нового документа
	virtual BOOL OnNewDocument();
	// процедура загрузки shape-файла
	virtual void Serialize(CArchive& ar);
	// удаляет все содержимое документа
	virtual void DeleteContents();

	/* загружает данные из Shape файла в структуру SMap.
	При удачной загрузке возвр. TRUE, при ошибке FALSE*/
	BOOL LoadFromFile(CArchive &ar, SMap &Layer, BOOL *pbAbortLoad);

private:
	CShapeViewerDoc();

	/* читаем фигуру (заголвок и содержимое) с текущей позиции в файле
	в структуру SMap.
    Эта ф-ия вспомогательная для LoadFromFile
	Текущая позиция файла должна быть установлена на начало записи */
	CShape* _ReadShape(CFile &file, SMap &Layer, BOOL *pbAbortLoad);

	/* читает из текущей позиции открытого 
	файла двойное слово в формате  Big-Indian 
	Если достигнут конец файла порождает исключение CFileException*/
	DWORD _ReadBigIndianDWORD(CFile &f);

	/* читает из текущей позиции открытого 
	файла двойное слово в формате  Little-Indian 
	Если достигнут конец файла порождает исключение CFileException*/
	DWORD _ReadLittleIndianDWORD(CFile &f);

	/*читает из текущей позиции открытого 
	файла в формате  Little-Indian переменную типа 64-bit IEEE double*/
	DOUBLE _ReadLittleIndianDOUBLE(CFile &f);

	/* читает из текущей позиции открытого файла
	содержимое записи c фигурой типа Point
	При ошибке 
	1 возвращает NULL
	2 показывает диалог с ошибкой и выбором: прервать загрузку или нет
	2 Если пользоватеь выбрал прервать загрузку файла *pAbortLoad == TRUE,
	  если нет - FALSE*/
	CDot* _ReadDot(CFile &f, SMap &Layer, INT iRecordNumber, 
					INT iContentLen, BOOL *pbAbortLoad, BOOL bSkipShapeType = FALSE);

	/* читает из текущей позиции открытого файла
	содержимое записи c фигурой типа Arc или Polygon,
	в зависимости от значения ShapeT
	При ошибке 
	1 возвращает NULL
	2 показывает диалог с ошибкой и выбором: прервать загрузку или нет
	2 Если пользоватеь выбрал прервать загрузку файла *pAbortLoad == TRUE,
	  если нет - FALSE*/
	CShape* _ReadArcPoly(CFile &f, SMap &Layer, EShapeType ShapeT, 
						INT iRecordNumber, INT iContentLen, BOOL *pbAbortLoad);

	/*Вспомогательная ф-ия для ф-ий ReadИмяФигуры
	Читает поле Shape Type в содержимом текущей записи
	Если считанный тип записи неравен St сообщает об ошибке и возвр. -1
	При ошибке возвращает -1*/
	INT _ReadShapeType(CFile &f, EShapeType St, INT iRecordNumber, INT iContentLen);

	/*Вспомогательная ф-ия для ф-ий ReadИмяФигуры
	Читает поле Bounding Box в содержимом текущей записи фигуры
	При ошибке возвращает FALSE, иначе - TRUE
	Если фигура находится вне Bounding Box'а карты выводит
	предупредительное сообщение */
	BOOL _ReadBoundingBox(CFile &f, INT iRecordNumber, INT iContentLen, SMap &Layer, CMyRect *pBBox);

	/* Выводит на экран сообщение об ощибке в загрузке файла. 
	Если ошибка критическая(IsCritical == TRUE), то 
	просто показывает сообщение с кнопкой OK и всегда возвр. FALSE
	Если ошика не критичкая (IsCritical == FALSE), 
	то показывает кнопки Yes и No и в конце сообщения приписывает
	"Прервать ли загрузку или нет?"
	Если пользователь выбирает Yes возвр. TRUE, No - FALSE*/
	BOOL _ReportError(CString ErMsg, UINT CaptionID, BOOL IsCritical = FALSE);

	/*Очищает карту, корректно освобождая память из-под
	диманически выделенных объектов*/
	void _DeleteShapes(SMap *pLayer);

	void _ConnectToDB(CFile &f, SMap *pLayer);

private:
	// очередь из слоев изображения. Хранит всю геометрию рисунка.
	deque<SMap> m_Layers;

	/*Bounding Box, который ограничивает все карты в документе
	от самой левой-верхней до правой-нижней*/
	CMyRect m_BoundBox;

	DECLARE_DYNCREATE(CShapeViewerDoc)
	DECLARE_MESSAGE_MAP()
};