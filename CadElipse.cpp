// CadElipse.cpp: implementation of the CCadElipse class.
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

CCadElipse::CCadElipse():CCadObject(OBJECT_TYPE_ELLIPSE)
{
}

CCadElipse::CCadElipse(CCadElipse &e):CCadObject(OBJECT_TYPE_ELLIPSE)
{
	SetP1(e.GetP1());
	SetP2(e.GetP2());
	SetLineColor(e.GetLineColor());
	SetFillColor(e.GetFillColor());
	SetLineWidth(e.GetLineWidth());	
	SetTransparent(e.GetTransparent());
}

CCadElipse::~CCadElipse()
{
}

void CCadElipse::Draw(CDC *pDC, int mode,CPoint Offset,CScale Scale)
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
	CBrush *pOldBr, brushFill, brushPoint;
	CRect rect;
	CSize rectLWcomp;
	CPoint P1,P2;
	CSize szDiff = CSize(4,4);
	int Lw;

	if (CCadElipse::m_RenderEnable)
	{
		P1 = Scale * GetP1() + Offset;
		P2 = Scale * GetP2() + Offset;
		if ((Lw = int(Scale.m_ScaleX * GetLineWidth())) < 1) 
			Lw = 1;
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
			penLine.CreatePen(PS_SOLID, Lw, GetLineColor());
			if(GetTransparent() || CCadObject::AreShapeFillsDisabled() )
				brushFill.CreateStockObject(NULL_BRUSH);
			else
				brushFill.CreateSolidBrush(GetFillColor());
			break;
		case OBJECT_MODE_SELECTED:
			penLine.CreatePen(PS_SOLID, Lw, RGB(50, 50, 200));
			if (GetTransparent() || CCadObject::AreShapeFillsDisabled())
				brushFill.CreateStockObject(NULL_BRUSH);
			else
				brushFill.CreateSolidBrush(RGB(150,150,255));
			break;
		case OBJECT_MODE_SKETCH:
			penLine.CreatePen(PS_DOT, 1, GetLineColor());
			brushFill.CreateStockObject(NULL_BRUSH);
			break;
		}
		brushPoint.CreateSolidBrush(RGB(0, 0, 255));
		penPoint.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		SetRect(rect, P1, P2, rectLWcomp);
		pOld = pDC->SelectObject(&penLine);
		pOldBr = pDC->SelectObject(&brushFill);
		switch (mode)
		{
		case OBJECT_MODE_FINAL:
			pDC->Ellipse(&rect);
			break;
		case OBJECT_MODE_SELECTED:
		{
			pDC->Ellipse(&rect);
			pDC->SelectObject(&penPoint);
			pDC->SelectObject(&brushPoint);
			rect.SetRect(P1 - szDiff, P1 + szDiff);
			pDC->Rectangle(&rect);
			rect.SetRect(P2 - szDiff, P2 + szDiff);
			pDC->Rectangle(&rect);
		}
		break;
		case OBJECT_MODE_SKETCH:
			pDC->Ellipse(&rect);
			break;
		case OBJECT_MODE_ERASE:
			break;
		}
		pDC->SelectObject(pOldBr);
		pDC->SelectObject(pOld);
	}
}

int CCadElipse::CheckSelected(CPoint p,CSize O)
{
	double a,b,xo,yo,v;
	int rV;
	static int count = 0;
	CPoint P1 = GetP1() + O;
	CPoint P2 = GetP2() + O;

	a = double(P2.x - P1.x)/2.0;
	b = double(P2.y - P1.y)/2.0;
	xo = p.x - (a + P1.x);
	yo = p.y - (b + P1.y);
	v = (xo * xo)/(a * a) + (yo * yo)/(b * b);
	if( v < 1.0) rV = TRUE;
	else rV = FALSE;
	return rV;
}

CCadElipse CCadElipse::operator=(CCadElipse &e)
{
	CCadElipse eNew;
	eNew.SetP1(e.GetP1());
	eNew.SetP2(e.GetP2());
	eNew.SetLineColor(e.GetLineColor());
	eNew.SetFillColor(e.GetFillColor());
	eNew.SetLineWidth(e.GetLineWidth());
	eNew.SetTransparent(e.GetTransparent());
	return eNew;
}

void CCadElipse::Move(CPoint p)
{
	CSize Diff = p - GetP1();
	SetP1(GetP1() + Diff);
	SetP2(GetP2() + Diff);
}

int CCadElipse::Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser)
{
	BOOL Loop = TRUE;

	LookAHeadToken = pParser->Expect(TOKEN_ELLIPSE, LookAHeadToken, pIN);
	LookAHeadToken = pParser->Expect('(', LookAHeadToken, pIN);
	while (Loop)
	{
		switch (LookAHeadToken)
		{
		case TOKEN_POINT_1:
			LookAHeadToken = pParser->Point(TOKEN_POINT_1, pIN, GetP1(), LookAHeadToken);
			break;
		case TOKEN_POINT_2:
			LookAHeadToken = pParser->Point(TOKEN_POINT_2, pIN, GetP2(), LookAHeadToken);
			break;
		case TOKEN_LINE_COLOR:
			LookAHeadToken = pParser->Color(TOKEN_LINE_COLOR, pIN, GetAttributes()->GetLineColorRef(), LookAHeadToken);
			break;
		case TOKEN_FILL_COLOR:
			LookAHeadToken = pParser->Color(TOKEN_FILL_COLOR, pIN, GetAttributes()->GetFillColorRef(), LookAHeadToken);
			break;
		case TOKEN_LINE_WIDTH:
			LookAHeadToken = pParser->DecimalValue(TOKEN_LINE_WIDTH, pIN, GetAttributes()->GetLineWidthRef(), LookAHeadToken);
			break;
		case TOKEN_TRANSPARENT:
			LookAHeadToken = pParser->DecimalValue(TOKEN_TRANSPARENT, pIN, GetAttributes()->GetTransparentRef(), LookAHeadToken);
			break;
		case ')':
			Loop = FALSE;
			LookAHeadToken = pParser->Expect(')', LookAHeadToken, pIN);
			break;
		case ',':
			LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
			break;
		default:
			break;
		}
	}
	(*ppDrawing)->AddObject(this);
	return LookAHeadToken;
}

void CCadElipse::Save(FILE* pO, int Indent)
{
	char* s = new char[256];
	char* s1 = new char[64];
	char* s2 = new char[64];
	char* s3 = new char[64];
	char* s4 = new char[64];
	char* s5 = new char[64];
	char* s6 = new char[64];

	fprintf(pO, "%s%s(%s,%s,%s,%s,%s, %s)\n",
		theApp.IndentString(s, 256, Indent),
		CFileParser::TokenLookup(TOKEN_ELLIPSE),
		CFileParser::SavePoint(s1, 64, TOKEN_POINT_1, GetP1()),
		CFileParser::SavePoint(s2, 64, TOKEN_POINT_2, GetP2()),
		CFileParser::SaveColor(s3, 64, GetAttributes()->GetLineColor(), TOKEN_LINE_COLOR),
		CFileParser::SaveColor(s4, 64, GetAttributes()->GetFillColor(), TOKEN_FILL_COLOR),
		CFileParser::SaveDecimalValue(s5,64, TOKEN_LINE_WIDTH, GetAttributes()->GetLineWidthRef()),
		CFileParser::SaveDecimalValue(s6, 64, TOKEN_TRANSPARENT, GetAttributes()->GetTransparentRef())
	);
	delete[] s6;
	delete[] s5;
	delete[] s4;
	delete[] s3;
	delete[] s2;
	delete[] s1;
	delete[] s;
}

int CCadElipse::GrabVertex(CPoint point)
{
	int rV = -1;
	CSize Diff(4,4);
	CRect rect;
	rect.SetRect(GetP1() + Diff, GetP1() - Diff);
	rect.NormalizeRect();
	if(rect.PtInRect(point))
		rV = 0;
	else
	{
		rect.SetRect(GetP2() + Diff, GetP2() - Diff);
		rect.NormalizeRect();
		if(rect.PtInRect(point))
			rV = 1;
	}
	return rV;
}

void CCadElipse::SetVertex(int Vi, CPoint p)
{
	if(Vi)
		SetP2(p);
	else
		SetP1(p);
}

CPoint CCadElipse::GetReference()
{
	return GetP1();
}

void CCadElipse::AdjustRefernce(CPoint p)
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

void CCadElipse::RenderEnable(int e)
{
	CCadElipse::m_RenderEnable = e;
}

CPoint CCadElipse::GetCenter()
{
	CRect rect = this->GetRect();
	return rect.CenterPoint();
}

// Moves the center of the object to the spcified point
void CCadElipse::ChangeCenter(CSize p)
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


CSize CCadElipse::GetSize()
{
	CRect rect = GetRect();
	return rect.Size();
}


void CCadElipse::ChangeSize(CSize Sz)
{
	SetP2(GetP2() + Sz);
}
