// CadLine.cpp: implementation of the CCadLine class.
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

CCadLine::CCadLine():CCadObject(OBJECT_TYPE_LINE)
{
	m_OutLineWidth = 1;
	m_LineColor = RGB(0,0,0);	//black line
	m_Poly = 0;
}

CCadLine::CCadLine(CCadLine &line):CCadObject(OBJECT_TYPE_LINE)
{
	m_OutLineWidth = 1;
	m_LineColor = line.m_LineColor;	//black line
	m_Poly = 0;
	SetP1(line.GetP1());
	SetP2(line.GetP2());
}

CCadLine::~CCadLine()
{
}

void CCadLine::Draw(CDC *pDC, int mode,CPoint O,CScale Scale)
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
	CPen *pOld, penLine, penPoint;
	LOGBRUSH lbMode;
	CBrush brushFill, * pOldB;
	CPoint P1,P2,Diff;
	int Lw;
	CRect rect;

	if (CCadLine::m_RenderEnable)
	{
		P1 = (Scale * GetP1()) + O;
		P2 = (Scale * GetP2()) + O;
		Lw = int(Scale.m_ScaleX * m_OutLineWidth);
		if (Lw < 1) Lw = 1;
		brushFill.CreateSolidBrush(RGB(0,0,255));
		switch (mode)
		{
		case OBJECT_MODE_FINAL:
			if(CCadObject::AreShapeFillsDisabled())
				lbMode.lbStyle = BS_HOLLOW;
			else
			{
				lbMode.lbStyle = BS_SOLID;
				lbMode.lbColor = m_LineColor;
				lbMode.lbHatch = 0;
			}
			penLine.CreatePen(PS_GEOMETRIC | PS_ENDCAP_ROUND | PS_JOIN_ROUND, Lw, &lbMode);
			break;
		case OBJECT_MODE_SELECTED:
			if (CCadObject::AreShapeFillsDisabled())
				lbMode.lbStyle = BS_HOLLOW;
			else
			{
				lbMode.lbStyle = BS_SOLID;
				lbMode.lbColor = RGB(255,0,0);
				lbMode.lbHatch = 0;
			}
			penLine.CreatePen(PS_GEOMETRIC | PS_ENDCAP_ROUND | PS_JOIN_ROUND, Lw, &lbMode);
			penPoint.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
			break;
		case OBJECT_MODE_SKETCH:
			penLine.CreatePen(PS_DOT , 1, RGB(0, 0, 255));
			break;
		}
		pOld = pDC->SelectObject(&penLine);
		pOldB = pDC->SelectObject(&brushFill);
		switch (mode)
		{
		case OBJECT_MODE_FINAL:
			pDC->MoveTo(P1);
			pDC->LineTo(P2);
			break;
		case OBJECT_MODE_SELECTED:
		case OBJECT_MODE_SKETCH:
			pDC->MoveTo(P1);
			pDC->LineTo(P2);
			Diff = CPoint(4, 4);
			rect.SetRect(P1 + (-Diff), P1 + Diff);
			pDC->SelectObject(&penPoint);
			pDC->Rectangle(&rect);
			rect.SetRect(P2 + (-Diff), P2 + Diff);
			pDC->Rectangle(&rect);
			break;
		case OBJECT_MODE_ERASE:
			break;
		}
		pDC->SelectObject(pOld);
		pDC->SelectObject(pOldB);
	}
}

int CCadLine::CheckSelected(CPoint p,CSize O)
{
	if(m_Poly == 0)
		m_Poly = new CCadPolygon(4);
	else
		m_Poly->Reset();

	//-----------------------------
	// enclose the line inside
	// of a polygon
	//-----------------------------
	CPoint P1 = GetP1() + O;
	CPoint P2 = GetP2() + O;
	int d = this->m_OutLineWidth/2;
	if(d < 10) d = 10;
	if((P1.x < P2.x) && (P1.y < P2.y))
	{
		m_Poly->AddPoint(CPoint(P1.x-d,P1.y+d));
		m_Poly->AddPoint(CPoint(P1.x+d,P1.y-d));
		m_Poly->AddPoint(CPoint(P2.x+d,P2.y-d));
		m_Poly->AddPoint(CPoint(P2.x-d,P2.y+d));
	}
	else if((P1.x > P2.x) && (P1.y < P2.y))
	{
		m_Poly->AddPoint(CPoint(P1.x-d,P1.y-d));
		m_Poly->AddPoint(CPoint(P1.x+d,P1.y+d));
		m_Poly->AddPoint(CPoint(P2.x+d,P2.y+d));
		m_Poly->AddPoint(CPoint(P2.x-d,P2.y-d));
	}
	else if((P1.x < P2.x) && (P1.y > P2.y))
	{
		m_Poly->AddPoint(CPoint(P1.x-d,P1.y-d));
		m_Poly->AddPoint(CPoint(P1.x+d,P1.y+d));
		m_Poly->AddPoint(CPoint(P2.x+d,P2.y+d));
		m_Poly->AddPoint(CPoint(P2.x-d,P2.y-d));
	}
	else if((P1.x > P2.x) && (P1.y > P2.y))
	{
		m_Poly->AddPoint(CPoint(P1.x-d,P1.y+d));
		m_Poly->AddPoint(CPoint(P1.x+d,P1.y-d));
		m_Poly->AddPoint(CPoint(P2.x+d,P2.y-d));
		m_Poly->AddPoint(CPoint(P2.x-d,P2.y+d));
	}
	else if((P1.x == P2.x)&& (P1.y < P2.y))
	{
		m_Poly->AddPoint(CPoint(P1.x-d,P1.y-d));
		m_Poly->AddPoint(CPoint(P1.x+d,P1.y-d));
		m_Poly->AddPoint(CPoint(P2.x+d,P2.y+d));
		m_Poly->AddPoint(CPoint(P2.x-d,P2.y+d));
	}
	else if((P1.x == P2.x)&& (P1.y > P2.y))
	{
		m_Poly->AddPoint(CPoint(P1.x-d,P1.y+d));
		m_Poly->AddPoint(CPoint(P1.x+d,P1.y+d));
		m_Poly->AddPoint(CPoint(P2.x+d,P2.y-d));
		m_Poly->AddPoint(CPoint(P2.x-d,P2.y-d));
	}
	else if((P1.x > P2.x)&& (P1.y == P2.y))
	{
		m_Poly->AddPoint(CPoint(P1.x+d,P1.y+d));
		m_Poly->AddPoint(CPoint(P1.x+d,P1.y-d));
		m_Poly->AddPoint(CPoint(P2.x-d,P2.y-d));
		m_Poly->AddPoint(CPoint(P2.x-d,P2.y+d));
	}
	else if((P1.x < P2.x)&& (P1.y == P2.y))
	{
		m_Poly->AddPoint(CPoint(P1.x-d,P1.y+d));
		m_Poly->AddPoint(CPoint(P1.x-d,P1.y-d));
		m_Poly->AddPoint(CPoint(P2.x+d,P2.y-d));
		m_Poly->AddPoint(CPoint(P2.x+d,P2.y+d));
	}
	return m_Poly->PointEnclosed(p);
}

int CCadLine::Parse(
	FILE* pIN,
	int LookAHeadToken,
	CCadDrawing** ppDrawing,
	CFileParser* pParser
)
{
	LookAHeadToken = pParser->Expect(TOKEN_LINE, LookAHeadToken, pIN);
	LookAHeadToken = pParser->Expect('(', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Point(TOKEN_POINT_1, pIN, GetP1(), LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Point(TOKEN_POINT_2, pIN, GetP2(), LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Color(TOKEN_LINE_COLOR, pIN, m_LineColor, LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->DecimalValue(TOKEN_LINE_WIDTH, pIN, m_OutLineWidth, LookAHeadToken);
	LookAHeadToken = pParser->Expect(')', LookAHeadToken, pIN);
	(*ppDrawing)->AddObject(this);
	return LookAHeadToken;
}

void CCadLine::Save(FILE *pO,  int Indent)
{
	char* s = new char[256];
	char* s1 = new char[64];
	char* s2 = new char[64];
	char* s3 = new char[64];
	char* s4 = new char[64];

	fprintf(pO, "%s%s(%s,%s,%s,%s)\n",
		theApp.IndentString(s, 256, Indent),
		CFileParser::TokenLookup(TOKEN_LINE),
		CFileParser::SavePoint(s1,64,TOKEN_POINT_1,GetP1()),
		CFileParser::SavePoint(s2, 64, TOKEN_POINT_2, GetP2()),
		CFileParser::SaveColor(s3,64,m_LineColor, TOKEN_LINE_COLOR),
		CFileParser::SaveDecimalValue(s4,64,TOKEN_LINE_WIDTH, m_OutLineWidth)
	);
	delete[] s4;
	delete[] s3;
	delete[] s2;
	delete[] s1;
	delete[] s;
}

CCadLine CCadLine::operator=(CCadLine &v)
{
	SetP1(v.GetP1());
	SetP2(v.GetP2());
	m_LineColor = v.m_LineColor;
	m_OutLineWidth = v.m_OutLineWidth;
	if((m_Poly == NULL) && (v.m_Poly != NULL))
		m_Poly = (CCadPolygon *)v.m_Poly->CopyObject();
	return *this;
}

void CCadLine::Move(CPoint p)
{
	CPoint Diff;
	Diff = p - GetP1();
	SetP1(GetP1() + Diff);
	SetP2(GetP2() + Diff);
}


int CCadLine::GrabVertex(CPoint point)
{
	int rV = -1;
	CRect rect;
	CPoint p1,p2,p;
	p = CPoint(4,4);
	p1 = GetP1() + p;
	p2 = GetP1() - p;
	rect.SetRect(p1,p2);
	rect.NormalizeRect();
	if(rect.PtInRect(point))
		rV = 0;
	else
	{
		p1 = GetP2() + p;
		p2 = GetP2() - p;
		rect.SetRect(p1,p2);
		rect.NormalizeRect();
		if(rect.PtInRect(point))
			rV = 1;
	}
	return rV;
}

void CCadLine::SetVertex(int Vi, CPoint p)
{
	if(Vi)
	{
		SetP2(p);
	}
	else
		SetP1(p);
}

CPoint CCadLine::GetReference()
{
	return GetP1();
}

void CCadLine::AdjustRefernce(CPoint p)
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

void CCadLine::RenderEnable(int e)
{
	CCadLine::m_RenderEnable = e;
}

CPoint CCadLine::GetCenter()
{
	CPoint Center((GetP1().x + GetP2().x) / 2, (GetP1().y + GetP2().y) / 2);
	return Center;
}

// Moves the center of the object to the spcified point
void CCadLine::ChangeCenter(CSize p)
{
}


CSize CCadLine::GetSize()
{
	return CSize(abs(GetP1().x - GetP2().x), abs(GetP1().y - GetP2().y));
}


void CCadLine::ChangeSize(CSize Sz)
{
	SetP2(GetP2() + Sz);
}
