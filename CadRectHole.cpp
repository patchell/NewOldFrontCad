// CadRectHole.cpp: implementation of the CCadRectHole class.
//
///////////////////////////////////////////////

#include "stdafx.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////

CCadRectHole::CCadRectHole():CCadObject(OBJECT_TYPE_HOLERECT)
{
	m_pPenLine = 0;
	m_H = 0;
	m_W = 0;
	m_Width = 0;
}


CCadRectHole::CCadRectHole(CCadRectHole &r):CCadObject(OBJECT_TYPE_HOLERECT)
{
	m_pPenLine=0;
	m_LineColor = r.m_LineColor;
	m_Width = r.m_Width;
	SetP1(r.GetP1());
	SetP2(r.GetP2());
	m_W = r.m_W;
	m_H = r.m_H;
}

CCadRectHole::~CCadRectHole()
{
	if(m_pPenLine) delete m_pPenLine;

}

void CCadRectHole::Draw(CDC *pDC, int mode,CPoint Offset,CScale Scale)
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
	int dx,dy;
	dx = m_W/2;
	dy = m_H/2;
	CPoint P1,P2;
	CRect rectHole;
	CBrush *pOldBrush = 0, brushRect;
	int Lw;

	if (CCadRectHole::m_RenderEnable)
	{
		brushRect.CreateSolidBrush(RGB(255,255,255));
		P1 = Scale * GetP1() + Offset;
		P2 = Scale * GetP2() + Offset;
		dx = int((Scale.m_ScaleX * m_W) / 2.0);
		dy = int((Scale.m_ScaleY * m_H) / 2.0);
		rectHole.SetRect(P1 + CPoint(-dx, -dy), P1 + CPoint(dx, dy));
		rectHole.NormalizeRect();
		rectHole.NormalizeRect();
		Lw = int(m_Width * Scale.m_ScaleX);
		if (Lw < 1) Lw = 1;

		if (GetLastMode() != mode || GetDirty())
		{
			if (m_pPenLine) delete m_pPenLine;
			switch (mode)
			{
			case OBJECT_MODE_FINAL:
				m_pPenLine = new CPen(PS_SOLID, Lw, m_LineColor);
				break;
			case OBJECT_MODE_SELECTED:
				m_pPenLine = new CPen(PS_SOLID, Lw, RGB(0, 250, 0));
				break;
			case OBJECT_MODE_SKETCH:
				m_pPenLine = new CPen(PS_SOLID, 1, m_LineColor);
				break;
			}
			SetDirty(0);
		}
		switch (mode)
		{
		case OBJECT_MODE_FINAL:
		case OBJECT_MODE_SELECTED:
		case OBJECT_MODE_SKETCH:
			pOld = pDC->SelectObject(m_pPenLine);
			pOldBrush = pDC->SelectObject(&brushRect);
//			pDC->MoveTo(P1 + CPoint(-dx, -dy));
//			pDC->LineTo(P1 + CPoint(-dx, dy));
//			pDC->LineTo(P1 + CPoint(dx, dy));
//			pDC->LineTo(P1 + CPoint(dx, -dy));
//			pDC->LineTo(P1 + CPoint(-dx, -dy));
			pDC->Rectangle(&rectHole);
			dx /= 2;
			dy /= 2;
			pDC->MoveTo(P1 + CPoint(dx, dy));
			pDC->LineTo(P1 + CPoint(-dx, -dy));
			pDC->MoveTo(P1 + CPoint(-dx, dy));
			pDC->LineTo(P1 + CPoint(dx, -dy));
			pDC->SelectObject(pOld);
			pDC->SelectObject(pOldBrush);
			break;
		case OBJECT_MODE_ERASE:
			break;
		}
		SetLastMode(mode);
	}
}

int CCadRectHole::CheckSelected(CPoint p,CSize O)
{
	int dx,dy;
	dx = m_W/2;
	dy = m_H/2;
	CPoint P1 = GetP1() + O;
	CRect rect(P1 + CPoint(-dx,-dy),P1 + CPoint(dx,dy));
	rect.NormalizeRect();
	return rect.PtInRect(p);
}

CCadRectHole CCadRectHole::operator=(CCadRectHole &v)
{
	SetP1(v.GetP1());
	SetP2(v.GetP2());
	m_H = v.m_H;
	m_W = v.m_W;
	m_LineColor = v.m_LineColor;
	m_Width = v.m_Width;
	return *this;
}


void CCadRectHole::Move(CPoint p)
{
	SetP1(p);
}

int CCadRectHole::Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser)
{
	LookAHeadToken = pParser->Expect(TOKEN_HOLERECT, LookAHeadToken, pIN);
	LookAHeadToken = pParser->Expect('(', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Point(TOKEN_POINT_1, pIN, GetP1(), LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->DecimalValue(TOKEN_RECTWIDTH, pIN, m_W, LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->DecimalValue(TOKEN_RECTHIEGHT, pIN, m_H, LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Color(TOKEN_LINE_COLOR, pIN, m_LineColor, LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->DecimalValue(TOKEN_LINE_WIDTH, pIN, m_Width, LookAHeadToken);
	LookAHeadToken = pParser->Expect(')', LookAHeadToken, pIN);
	(*ppDrawing)->AddObject(this);
	return LookAHeadToken;
}

void CCadRectHole::Save(FILE* pO, int Indent)
{
	char* s = new char[256];
	char* s1 = new char[64];
	char* s2 = new char[64];
	char* s3 = new char[64];
	char* s4 = new char[64];
	char* s5 = new char[64];

	fprintf(pO, "%sHOLERECT(%s,%s,%s,%s,%s)\n",
		theApp.IndentString(s, 256, Indent),
		CFileParser::SavePoint(s1, 64, TOKEN_POINT_1, GetP1()),
		CFileParser::SaveDecimalValue(s2, 64, TOKEN_RECTWIDTH, m_W),
		CFileParser::SaveDecimalValue(s3, 64, TOKEN_RECTHIEGHT, m_H),
		CFileParser::SaveColor(s4, 64, m_LineColor, TOKEN_LINE_COLOR),
		CFileParser::SaveDecimalValue(s5,64,TOKEN_LINE_WIDTH, m_Width)
	);
	delete[] s5;
	delete[] s4;
	delete[] s3;
	delete[] s2;
	delete[] s1;
	delete[] s;
}

int CCadRectHole::GrabVertex(CPoint point)
{
	int rV = -1;
	return rV;
}

void CCadRectHole::SetVertex(int Vi, CPoint p)
{

}

CPoint CCadRectHole::GetReference()
{
	return GetP1();
}

void CCadRectHole::AdjustRefernce(CPoint p)
{
	//-----------------------------------------
	//	AdjustRefernce
	//		Thhis function is used to normalize
	//	the location of points in the object
	// relative to a point choseen on the
	// drawing.
	//	parameters:
	//		p.....selected reference point
	//-----------------------------------------
	SetP1(GetP1() - p);
	SetP2(GetP2() - p);
}


CRect CCadRectHole::GetRect()
{
	CRect rect;
	CPoint p1,p2;
	int dx,dy;

	dx = this->m_W/2;
	dy = this->m_H/2;
	rect.SetRect(GetP1() + CPoint(dx, dy), GetP1() + CPoint(-dx, -dy));
	rect.NormalizeRect();
	return rect;
}

void CCadRectHole::RenderEnable(int e)
{
	CCadRectHole::m_RenderEnable = e;
}

CPoint CCadRectHole::GetCenter()
{
	return GetP1();
}

CPoint CCadRectHole::GetLowerRightPoint()
{
	CPoint pointCenter, pointLR;
	int cx, cy;
	
	pointCenter = GetP1();
	cx = m_W / 2;
	cy = m_H / 2;
	pointLR = pointCenter + CSize(cx, cy);
	return pointLR;
}

CPoint CCadRectHole::GetUpperLeftPoint()
{
	CPoint pointCenter, pointUL;
	int cx, cy;

	pointCenter = GetP1();
	cx = m_W / 2;
	cy = m_H / 2;
	pointUL = pointCenter + CSize(-cx, -cy);
	return pointUL;
}


int CCadRectHole::GetTop()
{
	int h, rV;
	CPoint pointCenter;

	pointCenter = GetP1();
	h = m_H / 2;
	rV = pointCenter.y - h;
	return rV;
}

int CCadRectHole::GetBottom()
{
	int h, rV;
	CPoint pointCenter;

	pointCenter = GetP1();
	h = m_H / 2;
	rV = pointCenter.y + h;
	return rV;
}

int CCadRectHole::GetLeft()
{
	int w, rV;
	CPoint pointCenter;

	pointCenter = GetP1();
	w = m_W / 2;
	rV = pointCenter.x - w;
	return rV;
}

int CCadRectHole::GetRight()
{
	int w, rV;
	CPoint pointCenter;

	pointCenter = GetP1();
	w = m_W / 2;
	rV = pointCenter.x + w;
	return rV;
}

// Moves the center of the object to the spcified point
void CCadRectHole::ChangeCenter(CSize p)
{
	//-----------------------------------------
	//	ChangeCenter
	//		Thhis function is used to normalize
	//	the location of points in the object
	// relative to a point choseen on the
	// drawing.
	//	parameters:
	//		p.....selected reference point
	//-----------------------------------------
	SetP1(GetP1() - p);
}


CSize CCadRectHole::GetSize()
{
	CRect rect = GetRect();
	return rect.Size();
}


void CCadRectHole::ChangeSize(CSize Sz)
{
	SetP2(GetP2() + Sz);
}

