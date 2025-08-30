#pragma once

struct CircleAttributes {
	int m_LineWidth;
	COLORREF m_LineColor;
	COLORREF m_FillColor;
	BOOL m_bTransparent;
	CircleAttributes() {
		m_LineWidth = 0;
		m_LineColor = RGB(0, 0, 0);
		m_FillColor = RGB(0, 0, 0);
		m_bTransparent = 0;
	}
};

class CCadCircle : public CCadObject
{
	inline static int m_RenderEnable = 1;
	CircleAttributes m_atrb;
	double m_Radius;
public:
	CCadCircle();
	CCadCircle(CCadCircle& e);
	virtual ~CCadCircle();
	static void SetRenderEnable(int e) { m_RenderEnable = e; }
	static int IsRenderEnabled() { return m_RenderEnable; }
	virtual void SetP2(CPoint p);
	void CalculateRadius();
	void SetRadius(int Radius);
	int GetRadius() { return int(m_Radius); }
	virtual CPoint GetReference();
	virtual void Move(CPoint p);
	virtual int Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser);
	virtual void Save(FILE* pO, int Indent);
	virtual int CheckSelected(CPoint p, CSize Offset = CSize(0, 0));
	virtual void Draw(CDC* pDC, int mode = 0, CPoint Offset = CPoint(0, 0), CScale Scale = CScale(0.1, 0.1));
	virtual void SetVertex(int Vi, CPoint p);
	virtual int GrabVertex(CPoint p);
	virtual void AdjustRefernce(CPoint Ref);
	void SetLineWidth(int w) { GetAttributes()->m_LineWidth = w; }
	int GetLineWidth(void) { return GetAttributes()->m_LineWidth; }
	void SetLineColor(COLORREF c) { GetAttributes()->m_LineColor = c; }
	COLORREF GetLineColor(void) { return GetAttributes()->m_LineColor; }
	void SetFillColor(COLORREF c) { GetAttributes()->m_FillColor = c; }
	COLORREF GetFillColor(void) { return GetAttributes()->m_FillColor; }
	void SetTransparent(BOOL bT) { GetAttributes()->m_bTransparent = bT; }
	BOOL GetTransparent() { return GetAttributes()->m_bTransparent; }
	CCadCircle operator=(CCadCircle& v);
	virtual void RenderEnable(int e);
	virtual CPoint GetCenter();
	// Moves the center of the object to the spcified point
	virtual void ChangeCenter(CSize p);
	virtual CSize GetSize();
	virtual void ChangeSize(CSize Sz);
	CircleAttributes* GetAttributes(void) { return &m_atrb; }
};

