// CadRoundRect.h: interface for the CCadRoundRect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CADROUNDRECT_H__F387B842_BCD2_4EB4_8F46_B8CBCE99BEB8__INCLUDED_)
#define AFX_CADROUNDRECT_H__F387B842_BCD2_4EB4_8F46_B8CBCE99BEB8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



typedef struct {
	COLORREF m_FillColor;
	COLORREF m_LineColor;
	int m_OutLineWidth;
	int m_Y3;
	int m_X3;
} RndRectAttb;

class CFileParser;

class CCadRoundRect : public CCadObject
{
	friend CFileParser;
	inline static int m_RenderEnable = 1;
	COLORREF m_FillColor;
	COLORREF m_LineColor;
	int m_OutLineWidth;
	CPoint m_P3;
	CPen *m_pPenLine;
public:
	CCadRoundRect(CCadRoundRect &r);
	CCadRoundRect();
	virtual ~CCadRoundRect();
	static void SetRenderEnable(int e) { m_RenderEnable = e; }
	static int IsRenderEnabled() { return m_RenderEnable; }
	virtual CPoint GetReference();
	virtual void Move(CPoint p);
	virtual int Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser);
	virtual void Save(FILE *pO,  int Indent);
	virtual void SetVertex(int Vi,CPoint p);
	virtual int GrabVertex(CPoint p);
	virtual void AdjustRefernce(CPoint Ref);
	CBrush * m_pBrush;
	virtual void Draw(CDC *pDC,int mode=0,CPoint Offset=CPoint(0,0),CScale Scale=CScale(0.1,0.1));
	virtual int CheckSelected(CPoint p, CSize Offset = CSize(0, 0));
	//------------------------------
	COLORREF GetFillColor(void){return m_FillColor;}
	COLORREF GetLineColor(void){return m_LineColor;}
	int GetOutLineWidth(void){return m_OutLineWidth;}
	void SetFillColor(COLORREF nC){m_FillColor = nC;}
	void SetLineColor(COLORREF nC){m_LineColor = nC;}
	void SetLineWidth(int nW){m_OutLineWidth = nW;}
	void SetArc(CPoint p){m_P3 = p;}
	CPoint GetArc(void){return m_P3;}
	CCadRoundRect operator=(CCadRoundRect &v);
	virtual void RenderEnable(int e);
	virtual CPoint GetCenter();
	// Moves the center of the object to the spcified point
	virtual void ChangeCenter(CSize p);
	virtual CSize GetSize();
	virtual void ChangeSize(CSize Sz);
};

#endif // !defined(AFX_CADROUNDRECT_H__F387B842_BCD2_4EB4_8F46_B8CBCE99BEB8__INCLUDED_)
