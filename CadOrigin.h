#pragma once


#ifndef ORIGINATRIB
#define ORIGINATRIB

typedef struct {
	int m_Width;
	COLORREF m_Color;
} OriginAttrib;

#endif

class CFrontCadView;

class CCadOrigin :public CCadObject
{
	inline static int m_RenderEnable = 1;
	OriginAttrib m_Atrib;
	CFrontCadView *m_pMainView;
	CPen *m_pPenLine;
public:
	CCadOrigin();
	CCadOrigin(CCadOrigin &v);
	virtual ~CCadOrigin();
	static void SetRenderEnable(int e) { m_RenderEnable = e; }
	static int IsRenderEnabled() { return m_RenderEnable; }
	virtual void Move(CPoint p);
	virtual int Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser);
	virtual void Save(FILE *pO,  int Indent);
	virtual void Draw(CDC *pDC, int mode = 0, CPoint Offset = CPoint(0, 0), CScale Scale = CScale(0.1, 0.1));
	virtual int CheckSelected(CPoint p, CSize Offset = CSize(0, 0));
	virtual CPoint GetReference();
	virtual CCadObject *GetHead(void) { return 0; }
	virtual void MakeDirty(void);
	virtual void SetSelected(int Flag = 0);
	virtual void AdjustRefernce(CPoint Ref);
	virtual CRect GetRect(void);
	inline void SetParent(CFrontCadView *pW) { m_pMainView = pW; }
	inline void SetColor(COLORREF c) { m_Atrib.m_Color = c; }
	inline COLORREF GetColor(void) { return m_Atrib.m_Color; }
	inline void SetWidth(int w) { m_Atrib.m_Width = w; }
	inline int GetWidth(void) { return m_Atrib.m_Width; }
	virtual void RenderEnable(int e);
	virtual CPoint GetCenter();
	// Moves the center of the object to the spcified point
	virtual void ChangeCenter(CSize p);
	virtual CSize GetSize();
	virtual void ChangeSize(CSize Sz);
};

