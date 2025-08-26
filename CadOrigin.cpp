#include "stdafx.h"

CCadOrigin::CCadOrigin() :CCadObject(OBJECT_TYPE_ORIGIN)
{
	m_pMainView = 0;
	m_pPenLine = 0;
	m_Atrib.m_Width = 1;
	m_Atrib.m_Color = RGB(0, 0, 0);
}

CCadOrigin::CCadOrigin(CCadOrigin &v) : CCadObject(OBJECT_TYPE_ORIGIN)
{
	m_pMainView = 0;
	m_pPenLine = 0;
	m_Atrib.m_Width = 1;
	m_Atrib.m_Color = v.m_Atrib.m_Color;
	SetP1(v.GetP1());
}

CCadOrigin::~CCadOrigin()
{
	if (m_pPenLine) delete m_pPenLine;
}

void CCadOrigin::Move(CPoint p)
{
	SetP1(p);
}

int CCadOrigin::Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser)
{
	LookAHeadToken = pParser->Expect(TOKEN_ORG, LookAHeadToken, pIN);
	LookAHeadToken = pParser->Expect('(', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Point(TOKEN_POINT_1, pIN, GetP1(), LookAHeadToken);
	LookAHeadToken = pParser->Expect(')', LookAHeadToken, pIN);
	(*ppDrawing)->AddObject(this);
	return LookAHeadToken;
}

void CCadOrigin::Save(FILE *pO,  int Indent)
{
	char *s = new char[256];
	char* s1 = new char[64];

	fprintf(pO, "%s%s(%s)\n", 
		theApp.IndentString(s,256,Indent),
		CFileParser::TokenLookup(TOKEN_ORG),
		CFileParser::SavePoint(s1,64,TOKEN_POINT_1,GetP1())
	);
	delete[] s1;
	delete[] s;
}

void CCadOrigin::Draw(CDC *pDC, int mode, CPoint Offset, CScale Scale)
{
	//---------------------------------------------
	//	Draw
	//		This function draws the object onto the
	//	specified device context.
	//
	// parameters:
	//		pDC......pointer to the device context
	//		mode.....mode to use when drawing
	//		Offset...Offset to add to points
	//		Scale....Sets Units to Pixels ratio
	//---------------------------------------------
	CPen *pOld;
	CRect rect;
	CPoint P1, P2;
	int Lw;

	if (CCadOrigin::m_RenderEnable)
	{
		P1 = (Scale * GetP1()) + Offset + CSize(10, 10);
		P2 = (Scale * GetP1()) + Offset - CSize(10, 10);
		Lw = int(Scale.m_ScaleX * m_Atrib.m_Width);
		if (Lw < 1) Lw = 1;
		rect.SetRect(P1, P2);
		rect.NormalizeRect();
		if ((GetLastMode() != mode) || GetDirty())
		{
			if (m_pPenLine) delete m_pPenLine;
			switch (mode)
			{
			case OBJECT_MODE_FINAL:
				m_pPenLine = new CPen(PS_SOLID, Lw, m_Atrib.m_Color);
				break;
			case OBJECT_MODE_SELECTED:
				m_pPenLine = new CPen(PS_SOLID, Lw, RGB(0, 255, 50));
				break;
			case OBJECT_MODE_SKETCH:
				m_pPenLine = new CPen(PS_DOT, 1, RGB(0, 0, 255));
				break;
			}
		}
		switch (mode)
		{
		case OBJECT_MODE_FINAL:
		case OBJECT_MODE_SELECTED:
		case OBJECT_MODE_SKETCH:
			pOld = pDC->SelectObject(m_pPenLine);
			pDC->Ellipse(&rect);
			P1 = (Scale * GetP1()) + Offset;
			pDC->MoveTo(P1.x, P1.y + 15);
			pDC->LineTo(P1.x, P1.y - 15);
			pDC->MoveTo(P1.x + 15, P1.y);
			pDC->LineTo(P1.x - 15, P1.y);
			pDC->SelectObject(pOld);
			break;
		case OBJECT_MODE_ERASE:
			break;
		}
		SetLastMode(mode);
	}
}

int CCadOrigin::CheckSelected(CPoint p,CSize O)
{
	int rV = 0;
	CRect rect;
	CPoint P1, P2;
	P1 = GetP1() + O;
	P2 = GetP1() + O;
	P1 += CSize(100, 100);
	P2 -= CSize(100, 100);
	rect.SetRect(P1, P2);
	rect.NormalizeRect();
	rV = rect.PtInRect(p);
	return rV;
}

CPoint CCadOrigin::GetReference()
{
	return GetP1();
}

void CCadOrigin::MakeDirty(void)
{
	SetDirty(1);
}

void CCadOrigin::SetSelected(int Flag)
{
	CCadObject::SetSelected(Flag);
}

void CCadOrigin::AdjustRefernce(CPoint Ref)
{
	SetP1(GetP1() - Ref);
	if (m_pMainView) m_pMainView->PostMessageA(WM_UPDATEDIMENSIONS);
}

CRect CCadOrigin::GetRect(void)
{
	CRect rR;
	CPoint P1, P2;
	P1 = GetP1() + CSize(100, 100);
	P2 = GetP1() - CSize(100, 100);
	rR.SetRect(P1, P2);
	rR.NormalizeRect();
	return rR;
}

void CCadOrigin::RenderEnable(int e)
{
	CCadOrigin::m_RenderEnable = e;
}

CPoint CCadOrigin::GetCenter()
{
	return GetP1();
}

// Moves the center of the object to the spcified point
void CCadOrigin::ChangeCenter(CSize p)
{
	SetP1(GetP1() - p);
}


CSize CCadOrigin::GetSize()
{
	CRect rect = GetRect();
	return rect.Size();
}


void CCadOrigin::ChangeSize(CSize Sz)
{
	SetP2(GetP2() + Sz);
}
