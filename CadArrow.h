#pragma once


struct ArrowAttributes {
	COLORREF m_Color;
	int m_Len;
	int m_ArrowWidth;
	ArrowAttributes() {
		m_Color = RGB(0, 0, 0);
		m_Len = 20;
		m_ArrowWidth = 10;
	}

};

class CFileParser;

class CCadArrow :public CCadObject
{
	friend CFileParser;
	inline static int m_RenderEnable = 1;
private:
	ArrowAttributes m_Attrib;
	CBrush *m_pBrFill;
	CPen *m_pPen;
	CPoint m_P3, m_P4;
private:	//methodes
	void CalcPoints(CPoint & a, CPoint & b, CPoint Offset, CScale Scale);
public:
	CCadArrow();
	CCadArrow(CCadArrow &ca);
	virtual ~CCadArrow();
	static void SetRenderEnable(int e) { m_RenderEnable = e; }
	static int IsRenderEnabled() { return m_RenderEnable; }
	virtual void Move(CPoint p);
	virtual void Save(FILE *pO,  int Indent);
	virtual int Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser);
	virtual void SetVertex(int Vi, CPoint p);
	virtual int GrabVertex(CPoint p);
	virtual void Draw(CDC *pDC, int mode = 0, CPoint Offset = CPoint(0, 0), CScale Scale = CScale(0.1, 0.1));
	virtual int CheckSelected(CPoint p, CSize Offset = CSize(0, 0));
	virtual CPoint GetReference();
	virtual void AddObject(CCadObject *pO);
	virtual void RemoveObject(CCadObject *pO);
	virtual CCadObject *GetHead(void);
	virtual void MakeDirty(void);
	virtual void SetSelected(int Flag = 0);
	virtual void AdjustRefernce(CPoint Ref);
	virtual CRect GetRect(void);
	COLORREF GetColor(void) const { return m_Attrib.m_Color; }
	void SetColor(COLORREF c) { m_Attrib.m_Color = c; }
	int GetWidth(void) { return m_Attrib.m_ArrowWidth; }
	void SetArrowWidth(int w) { m_Attrib.m_ArrowWidth = w; }
	int GetArrowLegnth(void) { return m_Attrib.m_Len; }
	void SetLegnth(int l) { m_Attrib.m_Len = l; }
	CCadArrow operator=(CCadArrow & Ca);
	virtual void RenderEnable(int e);
	virtual CPoint GetCenter();
	// Moves the center of the object to the spcified point
	virtual void ChangeCenter(CSize p);
	virtual CSize GetSize();
	virtual void ChangeSize(CSize Sz);
};

