#include "stdafx.h"


CCadCircle::CCadCircle() :CCadObject(OBJECT_TYPE_CIRCLE)
{
	m_Radius = 0.0;
	m_FillColor = RGB(0, 0, 0);
	m_LineColor = RGB(0, 0, 0);
	m_Width = 0;
	m_bTransparentFill = FALSE;
}

CCadCircle::CCadCircle(CCadCircle& e) :CCadObject(OBJECT_TYPE_CIRCLE)
{
	SetP1(e.GetP1());
	SetP2(e.GetP2());
	this->m_FillColor = e.m_FillColor;
	this->m_LineColor = e.m_LineColor;
	this->m_Width = e.m_Width;
	this->m_bTransparentFill = e.m_bTransparentFill;
	m_Radius = e.m_Radius;
}

CCadCircle::~CCadCircle()
{
}

void CCadCircle::SetP2(CPoint p)
{
	CCadObject::SetP2(p);
	CalculateRadius();
}

void CCadCircle::CalculateRadius()
{
	CSize szRadius;
	double r;

	szRadius = GetP2() - GetP1();
	r = double(szRadius.cx * szRadius.cx + szRadius.cy * szRadius.cy);;
	r = sqrt(r);
	m_Radius = r;
}

void CCadCircle::SetRadius(int R)
{
	CCadObject::SetP2(GetP1() + CSize(R, 0));
	m_Radius = double(R);
}

void CCadCircle::Draw(CDC* pDC, int mode, CPoint Offset, CScale Scale)
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
	// 
	// In CCadObject, P1 is the center
	// and P2 is the radius
	//---------------------------------------------
	CPen* pOldPen = 0, PenLine, PenSelRect;
	CBrush* pOldBrush, BrushFill, BrushSelRect;
	CRect rect;
	CSize rectLWcomp;
	CPoint P1, P2;
	CSize szRadius;
	double r = 0.0;

	int Lw;

	if (CCadCircle::m_RenderEnable)
	{
		P1 = Scale * GetP1() + Offset;
		P2 = Scale * GetP2() + Offset;
		szRadius = CSize(int(Scale.m_ScaleX * m_Radius), int(Scale.m_ScaleY * m_Radius));
		if ((Lw = int(Scale.m_ScaleX * m_Width)) < 1) Lw = 1;
		if (Lw <= 1 || OBJECT_MODE_SKETCH == mode)
		{
			Lw = 1;
			rectLWcomp = CSize(0, 0);
		}
		else
			rectLWcomp = CSize(Lw / 2, Lw / 2);
		switch (mode)
		{
		case OBJECT_MODE_FINAL:
			PenLine.CreatePen(PS_SOLID, Lw, m_LineColor);
			if (m_bTransparentFill)
				BrushFill.CreateStockObject(NULL_BRUSH);
			else
				BrushFill.CreateSolidBrush(m_FillColor);
			break;
		case OBJECT_MODE_SELECTED:
			PenLine.CreatePen(PS_SOLID, Lw, RGB(200, 50, 100));
			if (m_bTransparentFill)
				BrushFill.CreateStockObject(NULL_BRUSH);
			else
				BrushFill.CreateSolidBrush(RGB(255, 0, 0));
			PenSelRect.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
			BrushSelRect.CreateSolidBrush(RGB(0, 0, 255));
			break;
		case OBJECT_MODE_SKETCH:
			PenLine.CreatePen(PS_DOT, 1, m_LineColor);
			if (m_bTransparentFill)
				BrushFill.CreateStockObject(NULL_BRUSH);
			else
				BrushFill.CreateSolidBrush(m_FillColor);
			break;
		}
		SetRect(rect, P1 + szRadius, P1 - szRadius, rectLWcomp);
		pOldPen = pDC->SelectObject(&PenLine);
		pOldBrush = pDC->SelectObject(&BrushFill);
		switch (mode)
		{
		case OBJECT_MODE_FINAL:
			pDC->Ellipse(&rect);
			break;
		case OBJECT_MODE_SELECTED:
		{
			pDC->Ellipse(&rect);
			CSize p = CSize(4, 4);
			rect.SetRect(P1 - p, P1 + p);
			pDC->SelectObject(&PenSelRect);
			pDC->SelectObject(&BrushSelRect);
			pDC->Rectangle(&rect);
			rect.SetRect(P2 - p, P2 + p);
			pDC->Rectangle(&rect);
		}
		break;
		case OBJECT_MODE_SKETCH:
			pDC->Ellipse(&rect);
			break;
		case OBJECT_MODE_ERASE:
			break;
		}
		SetLastMode(mode);
		pDC->SelectObject(pOldPen);
		pDC->SelectObject(pOldBrush);
	}
}

int CCadCircle::CheckSelected(CPoint p, CSize O)
{
	int rV;
	static int count = 0;
	CSize szRadius;
	double Radius;
	int R;
	CRect Rect;

	CPoint P1 = GetP1() + O;
	CPoint P2 = GetP2() + O;
	szRadius = P2 - P1;
	Radius = szRadius.cx * szRadius.cx + szRadius.cy * szRadius.cy;
	R = int(sqrt(Radius));
	szRadius = CSize(R, R);
	Rect.SetRect(P1 + szRadius, P1 - szRadius);
	Rect.NormalizeRect();
	rV = Rect.PtInRect(p);
	return rV;
}

CCadCircle CCadCircle::operator=(CCadCircle& v)
{
	SetP1(v.GetP1());
	SetP2(v.GetP2());
	m_FillColor = v.m_FillColor;
	m_LineColor = v.m_LineColor;
	m_Width = v.m_Width;
	return *this;
}

void CCadCircle::Move(CPoint p)
{
	CSize Diff = p - GetP1();
	SetP1(GetP1() + Diff);
	SetP2(GetP2() + Diff);
}

int CCadCircle::Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser)
{
	LookAHeadToken = pParser->Expect(TOKEN_CIRCLE, LookAHeadToken, pIN);
	LookAHeadToken = pParser->Expect('(', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Point(TOKEN_POINT_1, pIN, GetP1(), LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Point(TOKEN_POINT_2, pIN, GetP2(), LookAHeadToken);
	CalculateRadius();
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Color(TOKEN_LINE_COLOR, pIN, m_LineColor, LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Color(TOKEN_FILL_COLOR, pIN, m_FillColor, LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->DecimalValue(TOKEN_LINE_WIDTH, pIN, m_Width, LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->DecimalValue(TOKEN_TRANSPARENT, pIN, m_bTransparentFill, LookAHeadToken);
	LookAHeadToken = pParser->Expect(')', LookAHeadToken, pIN);
	(*ppDrawing)->AddObject(this);
	return LookAHeadToken;
}

void CCadCircle::Save(FILE* pO, int Indent)
{
	char* s = new char[256];
	char* s1 = new char[64];
	char* s2 = new char[64];
	char* s3 = new char[64];
	char* s4 = new char[64];
	char* s5 = new char[64];
	char* s6 = new char[64];

	fprintf(theApp.GetLog(), "CCadCircle::Save  P2(%d,%d)\n", GetP2().x, GetP2().y);
	fprintf(pO, "%s%s(%s,%s,%s,%s,%s, %s)\n",
		theApp.IndentString(s, 256, Indent),
		CFileParser::TokenLookup(TOKEN_CIRCLE),
		CFileParser::SavePoint(s1, 64, TOKEN_POINT_1, GetP1()),
		CFileParser::SavePoint(s2, 64, TOKEN_POINT_2, GetP2()),
		CFileParser::SaveColor(s3, 64, m_LineColor, TOKEN_LINE_COLOR),
		CFileParser::SaveColor(s4, 64, m_FillColor, TOKEN_FILL_COLOR),
		CFileParser::SaveDecimalValue(s5, 64, TOKEN_LINE_WIDTH, m_Width),
		CFileParser::SaveTransparent(s6, 64, TOKEN_TRANSPARENT, m_bTransparentFill)
	);
	
	delete[] s6;
	delete[] s5;
	delete[] s4;
	delete[] s3;
	delete[] s2;
	delete[] s1;
	delete[] s;
}

int CCadCircle::GrabVertex(CPoint point)
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

void CCadCircle::SetVertex(int Vi, CPoint p)
{
	if (Vi)
		SetP2(p);
	else
		SetP1(p);
}

CPoint CCadCircle::GetReference()
{
	return GetP1();
}

void CCadCircle::AdjustRefernce(CPoint p)
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

void CCadCircle::RenderEnable(int e)
{
	CCadCircle::m_RenderEnable = e;
}

CPoint CCadCircle::GetCenter()
{
	CRect rect = this->GetRect();
	return rect.CenterPoint();
}

// Moves the center of the object to the spcified point
void CCadCircle::ChangeCenter(CSize p)
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
	SetP2(GetP2() - p);
}


CSize CCadCircle::GetSize()
{
	CRect rect = GetRect();
	return rect.Size();
}


void CCadCircle::ChangeSize(CSize Sz)
{
	SetP2(GetP2() + Sz);
}
