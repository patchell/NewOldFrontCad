// CadObject.h: interface for the CCadObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CADOBJECT_H__9AC56957_97CD_408B_B000_B41BC4FF00F9__INCLUDED_)
#define AFX_CADOBJECT_H__9AC56957_97CD_408B_B000_B41BC4FF00F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCadDrawing;
class CFileParser;
struct ObjectTypeMembers {
	const char* m_pName;
	int m_Type;
	ObjectTypeMembers()
	{
		m_pName = 0;
		m_Type = 0;
	}
	ObjectTypeMembers(const char* s, int T)
	{
		m_pName = s;
		m_Type = T;
	}
	BOOL Is(int Type) const {
		BOOL rV = FALSE;
		if (m_Type == Type)
			rV = TRUE;
		return rV;
	}
};
class CCadObject
{
	int m_Dirty;
	int m_LastMode;
	int m_Type;
	CPoint m_P2;
	CPoint m_P1;
	int m_Selected;
	CCadObject* m_pPrev;
	CCadObject* m_pNext;
	CCadObject* m_pSelNext;
	static BOOL RenderObjectFillsEnable;
public:
	CCadObject();
	CCadObject(int type);
	virtual ~CCadObject();
	//----------------------------------
	// Private Data Member Access
	//----------------------------------
	void SetDirty(int dirty) { m_Dirty = dirty; }
	int GetDirty() const { return m_Dirty; }
	void SetP1(CPoint p) { m_P1 = p; }
	void SetP1X(int x) { m_P1.x = x; }
	void SetP1Y(int y) { m_P1.y = y; }
	CPoint& GetP1() { return m_P1; }
	virtual void SetP2(CPoint p) { m_P2 = p; }
	void SetP2X(int x) { m_P2.x = x; }
	void SetP2Y(int y) { m_P2.y = y; }
	CPoint& GetP2() { return m_P2; }
	virtual void SetSelected(int Sel = 0) { m_Selected = Sel; }
	int GetSelected() const { return m_Selected; }
	void SetLastMode(int m) { m_LastMode = m; }
	int GetLastMode() const { return m_LastMode; }
	void SetNext(CCadObject* pN) { m_pNext = pN; }
	CCadObject* GetNext() { return m_pNext; }
	void SetPrev(CCadObject* pP) { m_pPrev = pP; }
	CCadObject* GetPrev() { return m_pPrev; }
	void SetNextSelection(CCadObject* pNS) { m_pSelNext = pNS; }
	CCadObject* GetNextSelection() { return m_pSelNext; }
	//----------------------------------
	virtual void Move(CPoint p);
	virtual int Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser);
	virtual void Save(FILE *pO, int Indent);
	virtual void SetVertex(int Vi,CPoint p);
	virtual int GrabVertex(CPoint p);
	virtual void Draw(CDC *pDC,int mode=0,CPoint Offset=CPoint(0,0),CScale Scale=CScale(0.1,0.1));
	virtual int CheckSelected(CPoint p,CSize Offset=CSize(0,0));
	virtual CPoint GetReference();
	virtual void AddObject(CCadObject *pO);
	virtual void RemoveObject(CCadObject *pO);
	virtual CCadObject *GetHead(void){return 0;}
	virtual void MakeDirty(void);
	virtual void AdjustRefernce(CPoint Ref);
	virtual CRect GetRect(void);
	virtual const char * GetTypeString(void);
	void SetType(int t){m_Type = t;}
	int GetType(void) const {return m_Type;}
	CCadObject operator=(CCadObject &v);
	CPoint ScalePoint(CPoint p,int Scale,int ScaleDiv);
	CCadObject * CopyObject(void);
	void SetRect(CRect & rect, CPoint P1, CPoint P2, CSize Lw);
	virtual void RenderEnable(int e);
	CCadObject * PopHead();
	CCadObject * PopTail();
	// Moves the center of the object to the spcified point
	virtual void ChangeCenter(CSize p);
	virtual CSize GetSize();
	virtual void ChangeSize(CSize Sz);
	static const char* LookupTypeName(int TypeToken);
	//-------------------------------------------------
	// Alignment
	//-------------------------------------------------
	virtual int GetTop();
	virtual int GetBottom();
	virtual int GetLeft();
	virtual int GetRight();
	virtual CPoint GetCenter();
	virtual CPoint GetUpperLeftPoint();
	virtual CPoint GetLowerRightPoint();
	static void EnableRenderFills() {
		RenderObjectFillsEnable = TRUE;
	}
	static void DisableRenderFills() {
		RenderObjectFillsEnable = FALSE;
	}
	static BOOL AreShapeFillsEnable() {
		return RenderObjectFillsEnable;
	}
	static BOOL AreShapeFillsDisabled() {
		return !RenderObjectFillsEnable;
	}
};

#endif // !defined(AFX_CADOBJECT_H__9AC56957_97CD_408B_B000_B41BC4FF00F9__INCLUDED_)
