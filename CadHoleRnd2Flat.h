// CadHoleRnd2Flat.h: interface for the CCadHoleRnd2Flat class.
//
///////////////////////////////////////////////

#if !defined(AFX_CADHOLERND2FLAT_H__EAD08037_7F47_4162_A518_74A7EF370B20__INCLUDED_)
#define AFX_CADHOLERND2FLAT_H__EAD08037_7F47_4162_A518_74A7EF370B20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

struct HoleRnd2FlatAttributes {
	int m_LineWidth;
	int m_FlatDist;	//distance from flat to center point
	COLORREF m_LineColor;
	int m_Radius;
	HoleRnd2FlatAttributes() {
		m_LineWidth = 1;
		m_LineColor = RGB(0, 0, 0);
		m_FlatDist = 50;
		m_Radius = 50;
	}
} ;

class CFileParser;

class CCadHoleRnd2Flat : public CCadObject
{
	friend CFileParser;
	inline static int m_RenderEnable = 1;
	HoleRnd2FlatAttributes m_atrb;
public:
	CCadHoleRnd2Flat(CCadHoleRnd2Flat & h);
	CCadHoleRnd2Flat();
	virtual ~CCadHoleRnd2Flat();
	static void SetRenderEnable(int e) { m_RenderEnable = e; }
	static int IsRenderEnabled() { return m_RenderEnable; }
	virtual CRect GetRect(void);
	virtual CPoint GetReference();
	virtual void Move(CPoint p);
	virtual int Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser);
	virtual void Save(FILE *pO,  int Indent);
	virtual int CheckSelected(CPoint p, CSize Offset = CSize(0, 0));
	virtual void SetVertex(int Vi,CPoint p);
	virtual int GrabVertex(CPoint p);
	double SolveIntersection(int m,CPoint P,int FlatDist,int Radius);
	virtual void Draw(CDC *pDC, int mode=0,CPoint Offset=CPoint(0,0),CScale Scale=CScale(0.1,0.1));
	virtual void AdjustRefernce(CPoint Ref);
	void SetLineWidth(int w){ GetAttributes()->m_LineWidth = w;}
	int GetLineWidth(void){return GetAttributes()->m_LineWidth;}
	void SetFlatDist(int d){ GetAttributes()->m_FlatDist = d;}
	int GetFlatDist(void){return GetAttributes()->m_FlatDist;}
	void SetRadius(int r){ GetAttributes()->m_Radius = r;}
	int GetRadius(void){return GetAttributes()->m_Radius;}
	void SetLineColor(COLORREF c){ GetAttributes()->m_LineColor = c;}
	COLORREF GetLineColor(void){return GetAttributes()->m_LineColor;}
	CCadHoleRnd2Flat operator=(CCadHoleRnd2Flat &v);
	virtual void RenderEnable(int e);
	virtual CPoint GetCenter();
	// Moves the center of the object to the spcified point
	virtual void ChangeCenter(CSize p);
	virtual CSize GetSize();
	virtual void ChangeSize(CSize Sz);
	HoleRnd2FlatAttributes* GetAttributes() { return &m_atrb; }
};

#endif // !defined(AFX_CADHOLERND2FLAT_H__EAD08037_7F47_4162_A518_74A7EF370B20__INCLUDED_)
