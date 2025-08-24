#include "stdafx.h"


CCadPrintRect::CCadPrintRect():CCadObject(OBJECT_TYPE_PRINTRECT)
{
	m_pPenLine = 0;
	m_Attrib.m_LineColor = 0;
	m_Attrib.m_Width = 0;
}

CCadPrintRect::CCadPrintRect(CCadPrintRect &PR) : CCadObject(OBJECT_TYPE_PRINTRECT)
{
	m_pPenLine = 0;
	m_Attrib.m_LineColor = PR.m_Attrib.m_LineColor;
	m_Attrib.m_Width = PR.m_Attrib.m_Width;
	SetP1(PR.GetP1());
	SetP2(PR.GetP2());
}


CCadPrintRect::~CCadPrintRect()
{
	if (m_pPenLine) delete m_pPenLine;
}


void CCadPrintRect::Draw(CDC *pDC, int mode, CPoint Offset, CScale Scale)
{
	///---------------------------------------------
	///	Draw
	///		This function draws the object onto the
	///	specified device context.
	///
	/// parameters:
	///		pDC......pointer to the device context
	///		mode.....mode to use when drawing
	///		Offset...Offset to add to points
	///		Scale....Sets Units to Pixels ratio
	///---------------------------------------------
	CPen *pOld;
	CRect rect;
	CSize rectLWcomp;
	CPoint P1, P2;
	int Lw;

	if (!CCadPrintRect::m_RenderEnable) return;	//don't print
	P1 = Scale * GetP1() + Offset;
	P2 = P1 + Scale * m_Size;;
	Lw = int(m_Attrib.m_Width * Scale.m_ScaleX);
	if (Lw <= 1 || OBJECT_MODE_SKETCH == mode)
	{
		Lw = 1;
		rectLWcomp = CSize(0, 0);
	}
	else
		rectLWcomp = CSize(Lw / 2, Lw / 2);
	if (GetLastMode() != mode || GetDirty())
	{
		if (m_pPenLine) delete m_pPenLine;
		switch (mode)
		{
		case OBJECT_MODE_FINAL:
			m_pPenLine = new CPen(PS_SOLID, Lw, m_Attrib.m_LineColor);
			break;
		case OBJECT_MODE_SELECTED:
			m_pPenLine = new CPen(PS_SOLID, Lw, RGB(200, 50, 50));
			break;
		case OBJECT_MODE_SKETCH:
			m_pPenLine = new CPen(PS_DOT, 1, m_Attrib.m_LineColor);
			break;
		}
	}
	if (GetDirty())
	{
		SetDirty(0);
	}
	switch (mode)
	{
	case OBJECT_MODE_FINAL:
		pOld = pDC->SelectObject(m_pPenLine);
		pDC->MoveTo(P1);
		pDC->LineTo(P1.x, P2.y);
		pDC->LineTo(P2);
		pDC->LineTo(P2.x, P1.y);
		pDC->LineTo(P1);
		pDC->SelectObject(pOld);
		break;
	case OBJECT_MODE_SELECTED:
	{
		CPen SelPen;
		CBrush SelBrush;
		SelPen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
		SelBrush.CreateSolidBrush(RGB(255, 0, 0));
		pOld = pDC->SelectObject(m_pPenLine);
		pDC->MoveTo(P1);
		pDC->LineTo(P1.x, P2.y);
		pDC->LineTo(P2);
		pDC->LineTo(P2.x, P1.y);
		pDC->LineTo(P1);
		pDC->SelectObject(&SelPen);
		pDC->SelectObject(&SelBrush);
		CSize p = CSize(4, 4);
		rect.SetRect(P1 - p, P1 + p);
		pDC->Rectangle(&rect);
		rect.SetRect(P2 - p, P2 + p);
		pDC->Rectangle(&rect);
		pDC->SelectObject(pOld);
	}
	break;
	case OBJECT_MODE_SKETCH:
		pOld = pDC->SelectObject(m_pPenLine);
		pDC->MoveTo(P1);
		pDC->LineTo(P1.x, P2.y);
		pDC->LineTo(P2);
		pDC->LineTo(P2.x, P1.y);
		pDC->LineTo(P1);
		pDC->SelectObject(pOld);
		break;
	case OBJECT_MODE_ERASE:
		break;
	}
	SetLastMode(mode);
}

int CCadPrintRect::CheckSelected(CPoint p,CSize O)
{
	CRect rect;
	int rV;
	CPoint P1 = GetP1() + O;
	CPoint P2 = P1 + m_Size;
	rect.SetRect(P1, P2);
	rect.NormalizeRect();
	rV = (int)rect.PtInRect(p);
	return rV;
}

int CCadPrintRect::Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser)
{
	LookAHeadToken = pParser->Expect(TOKEN_PRINTRECT, LookAHeadToken, pIN);
	LookAHeadToken = pParser->Expect('(', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Point(TOKEN_POINT_1, pIN, GetP1(), LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Size(TOKEN_SIZE, pIN, m_Size, LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->DecimalValue(TOKEN_LINE_WIDTH, pIN, m_Attrib.m_Width, LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Color(TOKEN_LINE_COLOR, pIN, m_Attrib.m_LineColor, LookAHeadToken);
	LookAHeadToken = pParser->Expect(')', LookAHeadToken, pIN);
	(*ppDrawing)->AddObject(this);
	return LookAHeadToken;
}

void CCadPrintRect::Save(FILE *pO,  int Indent)
{
	char* s = new char[256];
	char* s1 = new char[64];
	char* s2 = new char[64];
	char* s3 = new char[64];
	char* s4 = new char[64];

	fprintf(pO, "%s%s(%s,%s,%s,%s)\n",
		theApp.IndentString(s, 256, Indent),
		CFileParser::TokenLookup(TOKEN_PRINTRECT),
		CFileParser::SavePoint(s1, 64, TOKEN_POINT_1, GetP1()),
		CFileParser::SaveSize(s2, 64, TOKEN_SIZE, m_Size),
		CFileParser::SaveDecimalValue(s3,64,TOKEN_LINE_WIDTH,m_Attrib.m_Width),
		CFileParser::SaveColor(s4,64, m_Attrib.m_LineColor,TOKEN_LINE_COLOR)
	);
	delete[]s4;
	delete[]s3;
	delete[]s2;
	delete[]s1;
	delete[]s;
}

CCadPrintRect CCadPrintRect::operator=(CCadPrintRect &v)
{
	SetP1(v.GetP1());
	SetP2(v.GetP2());
	m_Attrib.m_LineColor = v.m_Attrib.m_LineColor;
	m_Attrib.m_Width = v.m_Attrib.m_Width;
	return *this;
}


void CCadPrintRect::Move(CPoint p)
{
	CSize Diff = p - GetP1();
	SetP1(GetP1() + Diff);
}

int CCadPrintRect::GrabVertex(CPoint point)
{
	int rV = -1;
	CSize Diff(4, 4);
	CRect rect;
	rect.SetRect(GetP1() + Diff, GetP1() - Diff);
	rect.NormalizeRect();
	if (rect.PtInRect(point))
		rV = 0;
	else
	{
		rect.SetRect(GetP2() + Diff, GetP2() - Diff);
		rect.NormalizeRect();
		if (rect.PtInRect(point))
			rV = 1;
	}
	return rV;
}

void CCadPrintRect::SetVertex(int Vi, CPoint p)
{
	if (Vi)
		SetP2(p);
	else
		SetP1(p);
}

CPoint CCadPrintRect::GetReference()
{
	return GetP1();
}

void CCadPrintRect::AdjustRefernce(CPoint p)
{
	///-----------------------------------------
	///	AdjustRefernce
	///		Thhis function is used to normalize
	///	the location of points in the object
	/// relative to a point choseen on the
	/// drawing.
	///	parameters:
	///		p.....selected reference point
	///-----------------------------------------
	SetP1(GetP1() - p);
}

CRect CCadPrintRect::GetRect()
{
	CRect rect;

	rect.SetRect(GetP1(), GetP1() + m_Size);
	rect.NormalizeRect();
	return rect;
}
void CCadPrintRect::RenderEnable(int e)
{
	CCadPrintRect::m_RenderEnable = e;
}

CPoint CCadPrintRect::GetCenter()
{
	CRect rect = this->GetRect();
	return rect.CenterPoint();
}

// Moves the center of the object to the spcified point
void CCadPrintRect::ChangeCenter(CSize p)
{
	///-----------------------------------------
	///	ChangeCenter
	///		Thhis function is used to normalize
	///	the location of points in the object
	/// relative to a point choseen on the
	/// drawing.
	///	parameters:
	///		p.....selected reference point
	///-----------------------------------------
	SetP1(GetP1() - p);
}

void CCadPrintRect::ChangeSize(CSize Sz)
{
	SetP2(GetP2() - Sz);
}
