#pragma once

/* класс представления. Отвечает за отображение карты в окне
и пользовательский интерфейс для работы с картой */
class CShapeViewerView : public CScrollView {
public:
	virtual ~CShapeViewerView();

private: // create from serialization only
	CShapeViewerView();
	DECLARE_DYNCREATE(CShapeViewerView)

private: // перегруженные методы
	// отрисовка окна
	virtual void OnDraw(CDC* pDC);

	/* задает размер scroll bar'а в соответствии с
	размером карты и масштабом */
	void UpdateScrollBars();

	/* ф-ия для связи с документом. Возвращает указатель на документ.*/
	CShapeViewerDoc* GetDocument() const;

	/* вызывается когда документ был модифицирован,
	с помощью CDocument::UpdateAllViews и нужна, чтоб отобразить 
	все изменения документа на экране */
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
private: // методы
	/* устанавливает заданный коэффициент масштабирования
	в combo box'е с масштабами, предварительно преобразуя
	вещественное число дробь в проценты */
	void _SetZoomInComboBox(DOUBLE dK);

	/* двигает scroll bar'ы представления так, чтобы масштабирование
	осуществлялось относительно центра окна. 
	Вызывается после масштабирования*/
	void _MoveScrollBarsAfterZoom(CSize PreZoomSBSize, CSize PostZoomSBSize, 
		CPoint PreZoomSBPos);

	/* возвращает размеры scroll bar'ов представления. 
	Если какого-либо scroll bar'а нет, возвр. 0 для соотв. 
	значения CSize*/
	CSize _GetScrollBarSizes();

	// обработчик события заливки окна перед отрисовкой
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

private:	// обработчики сообщений
	// обработчик события увеличения масштаба
	afx_msg void _OnZoomIn();

	// обработчик события уменьшения масштаба
	afx_msg void _OnZoomOut();

	// обработчик события восстановления оригинального размера карт
	afx_msg void _OnRestoreZoom();

	// обработчик события выравнивания размера карт по размеру окна
	afx_msg void _OnFitZoom();

	// обработчик события выбора масштаба из combo box'а с масштабами
	afx_msg void _OnCmbZoomSel();

	// обработчик события редактирования поля ввода combo box'а с масштабами
	afx_msg void _OnCmbZoomEditChange();

	// реакция на сообщение о смене курсора
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	// обработчик клика левой кнопки мыши на представлении
	afx_msg void OnLButtonDown( UINT, CPoint);

	// обработчик клика средней кнопки мыши (нажатие на колесо) на представлении
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	
	/* обработчик события движения мыши над представлением
	Двигает scroll bar'ы вслед за мышью, реализуя "перетаскивание" мышью*/
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// обработчик события поворота колеса мыши
	afx_msg BOOL OnMouseWheel(UINT nFlags, SHORT zDelta, CPoint pt);

	// обработчик события добавления нового слоя
	afx_msg void _OnAddLayer();

	// обработчик события удаления текущего слоя
	afx_msg void _OnDelLayer();

	// обработчик события выбора цвета отрисовки для текущего слоя
	afx_msg void _OnSelLayerColor();

	// обработчик события перемещения слоя вверх в порядке отрисовки
	afx_msg void _OnLayerUp();

	// обработчик события перемещения слоя вниз в порядке отрисовки
	afx_msg void _OnLayerDown();

	// обработчик события выбора слоя из ListView'а со слоями
	afx_msg void _OnLstLayersClick(NMHDR *pNotifyStruct, LRESULT *pLRes);

	// обработчик события изменения chek state у chek box'а "Видимый"
	afx_msg void _OnChkVisibleClick();

	/* обработчик элемента Меню Вид-Масштаб
	Если диалог масштаба виден - прячет его,
	если нет - показывает*/
	afx_msg void _OnMnuZoom();

private: // поля
	// координаты курсора в момент когда нажата кнопка "перетягивания" карты
	CPoint m_pntStartCurPos;

	/* текущий масштаб для всех слоев. 
	При масштабировании координаты вершин умножаются на это число*/
	DOUBLE m_dScale;

	// инициализирован ли combo box с выбором масштаба
	BOOL bCmbInited;

private:	// карта собщений
	DECLARE_MESSAGE_MAP()
};