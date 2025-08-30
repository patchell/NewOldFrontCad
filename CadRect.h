// CadRect.h: interface for the CCadRect class.
//
///////////////////////////////////////////////

#if !defined(AFX_CADRECT_H__65FA097A_DD30_4DCD_A691_E90895F5F2BC__INCLUDED_)
#define AFX_CADRECT_H__65FA097A_DD30_4DCD_A691_E90895F5F2BC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



struct RectAttributes {
	int m_LineWidth;
	COLORREF m_LineColor;
	COLORREF m_FillColor;
	BOOL m_bTransparentFill;
	RectAttributes() {
		m_LineWidth = 0;
		m_LineColor = RGB(0, 0, 0);
		m_FillColor = RGB(0, 0, 0);
		m_bTransparentFill = FALSE;
	}
};

class CFileParser;

class CCadRect : public CCadObject
{
	friend CFileParser;
	RectAttributes m_atrb;
	inline static int m_RenderEnable = 1;
public:
	CCadRect();
	CCadRect(CCadRect &r);
	virtual ~CCadRect();
	static void SetRenderEnable(int e) { m_RenderEnable = e; }
	static int IsRenderEnabled() { return m_RenderEnable; }
	virtual void Draw(CDC *pDC, int mode = 0, CPoint Offset = CPoint(0, 0), CScale Scale = CScale(0.1, 0.1));
	virtual int CheckSelected(CPoint p, CSize Offset = CSize(0, 0));
	virtual void AdjustRefernce(CPoint Ref);
	virtual void Move(CPoint p);
	virtual int Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser);
	virtual void Save(FILE *pO,  int Indent);
	virtual void SetVertex(int Vi, CPoint p);
	virtual int GrabVertex(CPoint p);
	void SetLineWidth(int w) { GetAttributes()->m_LineWidth = w; }
	int GetLineWidth(void) { return GetAttributes()->m_LineWidth; }
	void SetLineColor(COLORREF c) { GetAttributes()->m_LineColor = c; }
	COLORREF GetLineColor(void) { return GetAttributes()->m_LineColor; }
	void SetFillColor(COLORREF c) { GetAttributes()->m_FillColor = c; }
	COLORREF GetFillColor(void) { return GetAttributes()->m_FillColor; }
	CPoint GetReference();
	CCadRect operator=(CCadRect &v);
	virtual void RenderEnable(int e);
	virtual CPoint GetCenter();
	// Moves the center of the object to the spcified point
	virtual void ChangeCenter(CSize p);
	virtual CSize GetSize();
	virtual void ChangeSize(CSize Sz);
	RectAttributes* GetAttributes() { return &m_atrb; }
};

#endif // !defined(AFX_CADRECT_H__65FA097A_DD30_4DCD_A691_E90895F5F2BC__INCLUDED_)
