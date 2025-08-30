#include "stdafx.h"

CCadArrow::CCadArrow() :CCadObject(OBJECT_TYPE_ARROW)
{
	m_Attrib.m_Len = 0;
	m_Attrib.m_ArrowWidth = 0;
	m_Attrib.m_Color = RGB(0, 0, 0);
}

CCadArrow::CCadArrow(CCadArrow &ca):CCadObject(OBJECT_TYPE_ARROW)
{
	m_Attrib.m_Len = ca.m_Attrib.m_Len;
	m_Attrib.m_ArrowWidth = ca.m_Attrib.m_ArrowWidth;
	m_Attrib.m_Color = ca.m_Attrib.m_Color;
	SetP1(ca.GetP1());
	SetP2(ca.GetP2());
}


CCadArrow::~CCadArrow()
{
}

void CCadArrow::Move(CPoint p)
{
	CSize Diff = p - GetP1();
	SetP1(GetP1() + Diff);
	SetP2(GetP2() + Diff);
}

void CCadArrow::Save(FILE *pO,  int Indent)
{
	char* s = new char[256];
	char* s1 = new char[64];
	char* s2 = new char[64];
	char* s3 = new char[64];
	char* s4 = new char[64];
	char* s5 = new char[64];
	char* s6 = new char[64];

	fprintf(pO, "%s%s(%s,%s,%s,%s,%s,%s)\n",
		theApp.IndentString(s,256,Indent),
		CFileParser::TokenLookup(TOKEN_ARROW),
		CFileParser::SavePoint(s1, 64, TOKEN_POINT_1, GetP1()),
		CFileParser::SavePoint(s2, 64, TOKEN_POINT_2, GetP2()),
		CFileParser::SaveDecimalValue(s4,64,TOKEN_ARROW_LENGTH, m_Attrib.m_Len),
		CFileParser::SaveDecimalValue(s5, 64, TOKEN_ARROW_WIDTH, m_Attrib.m_ArrowWidth),
		CFileParser::SaveColor(s3,64, m_Attrib.m_Color, TOKEN_COLOR),
		CFileParser::SaveDecimalValue(s4, 64, TOKEN_TRANSPARENT, m_Attrib.m_bTransparent)
	);
	delete[]s6;
	delete[]s5;
	delete[]s4;
	delete[]s3;
	delete[]s2;
	delete[]s1;
	delete[]s;
}

int CCadArrow::Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser)
{
	BOOL bLoop = TRUE;

	LookAHeadToken = pParser->Expect(TOKEN_ARROW, LookAHeadToken, pIN);
	LookAHeadToken = pParser->Expect('(', LookAHeadToken, pIN);
	while (bLoop)
	{
		switch (LookAHeadToken)
		{
		case TOKEN_POINT_1:
			LookAHeadToken = pParser->Point(TOKEN_POINT_1, pIN, GetP1(), LookAHeadToken);
			break;
		case TOKEN_POINT_2:
			LookAHeadToken = pParser->Point(TOKEN_POINT_2, pIN, GetP2(), LookAHeadToken);
			break;
		case TOKEN_ARROW_LENGTH:
			LookAHeadToken = pParser->DecimalValue(TOKEN_ARROW_LENGTH, pIN, m_Attrib.m_Len, LookAHeadToken);
			break;
		case TOKEN_ARROW_WIDTH:
			LookAHeadToken = pParser->DecimalValue(TOKEN_ARROW_WIDTH, pIN, m_Attrib.m_ArrowWidth, LookAHeadToken);
			break;
		case TOKEN_COLOR:
			LookAHeadToken = pParser->Color(TOKEN_COLOR, pIN, m_Attrib.m_Color, LookAHeadToken);
			break;
		case TOKEN_TRANSPARENT:
			LookAHeadToken = pParser->DecimalValue(TOKEN_TRANSPARENT, pIN, m_Attrib.m_bTransparent, LookAHeadToken);
			break;
		case ',':
			LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
			break;
		case ')':
			bLoop = FALSE;
			LookAHeadToken = pParser->Expect(')', LookAHeadToken, pIN);
			break;
		default:
			break;
		}
	}
	(*ppDrawing)->AddObject(this);
	return LookAHeadToken;
}

void CCadArrow::SetVertex(int Vi, CPoint p)
{
	if (Vi)
		SetP2(p);
	else
		SetP1(p);
}

int CCadArrow::GrabVertex(CPoint p)
{
	return -1;
}

void CCadArrow::Draw(CDC *pDC, int mode, CPoint Offset, CScale Scale)
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
	CPen *penOLD = 0, penLine, penPoint;
	CBrush *brushOld = 0, brushFill, brushPoint;
	CRect rect;
	CPoint P1, P2,P3,P4;
	CSize szDiff = CSize(4,4);

	if (CCadArrow::m_RenderEnable)
	{
		P1 = Scale * GetP1() + Offset;
		P2 = Scale * GetP2() + Offset;
		if (mode == OBJECT_MODE_FINAL)
			CalcPoints(P3, P4, Offset, Scale);
		else
			CalcPoints(P3, P4, Offset, Scale);
		CPoint *pP = new CPoint[3];
		pP[0] = P1;
		pP[1] = P3;
		pP[2] = P4;

		switch (mode)
		{
		case OBJECT_MODE_FINAL:
			penLine.CreatePen(PS_SOLID, 1, m_Attrib.m_Color);
			if (CCadObject::AreShapeFillsDisabled())
				brushFill.CreateStockObject(NULL_BRUSH);
			else
				brushFill.CreateSolidBrush(m_Attrib.m_Color);
			break;
		case OBJECT_MODE_SELECTED:
			penLine.CreatePen(PS_SOLID, 1, m_Attrib.m_Color ^ 0x00ff00);
			brushFill.CreateSolidBrush(m_Attrib.m_Color ^ 0x00ff00);
			break;
		case OBJECT_MODE_SKETCH:
			penLine.CreatePen(PS_SOLID, 1, m_Attrib.m_Color);
			brushFill.CreateStockObject(NULL_BRUSH);
			break;
		}
		brushPoint.CreateSolidBrush(RGB(0, 0, 255));
		penPoint.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		penOLD = pDC->SelectObject(&penLine);
		brushOld = pDC->SelectObject(&brushFill);
		switch (mode)
		{
		case OBJECT_MODE_FINAL:
			pDC->Polygon(pP, 3);
			break;
		case OBJECT_MODE_SELECTED:
			pDC->Polygon(pP, 3);
			pDC->SelectObject(&penPoint);
			pDC->SelectObject(&brushPoint);
			pDC->Rectangle(CRect(pP[0] - szDiff, pP[0] + szDiff));
			pDC->Rectangle(CRect(pP[1] - szDiff, pP[1] + szDiff));
			pDC->Rectangle(CRect(pP[2] - szDiff, pP[2] + szDiff));
			break;
		case OBJECT_MODE_SKETCH:
			pDC->Polygon(pP, 3);
			pDC->SelectObject(&penPoint);
			pDC->SelectObject(&brushPoint);
			pDC->Rectangle(CRect(pP[0] - szDiff, pP[0] + szDiff));
			pDC->Rectangle(CRect(pP[1] - szDiff, pP[1] + szDiff));
			pDC->Rectangle(CRect(pP[2] - szDiff, pP[2] + szDiff));
			break;
		case OBJECT_MODE_ERASE:
			break;
		}
		pDC->SelectObject(penOLD);
		pDC->SelectObject(brushOld);
	}
}

int CCadArrow::CheckSelected(CPoint p,CSize Off)
{
	int rV = 0;

	if(theApp.HasConsol())
	{
		printf("------------------------------------------\n");
		printf("Enter CCadArrow::CheckSelected\n");
	}
	CCadPolygon Cp;
	Cp.AddPoint(GetP1() + Off, TRUE, TRUE);
	CPoint P3, P4;
	CalcPoints(P3, P4, Off, CScale(1.0, 1.0));
	Cp.AddPoint(P3, TRUE, TRUE);
	Cp.AddPoint(P4, TRUE, TRUE);
	rV = Cp.CheckSelected(p, CSize(0, 0));
	if (theApp.HasConsol())
	{
		printf("Exit CCadArrow::CheckSelected\n");
	}
	return rV;
}

CPoint CCadArrow::GetReference()
{
	return GetP1();
}

void CCadArrow::AddObject(CCadObject *pO)
{

}

void CCadArrow::RemoveObject(CCadObject *pO)
{

}

CCadObject *CCadArrow::GetHead(void)
{
	return 0;
}

void CCadArrow::SetSelected(int Flag)
{
	CCadObject::SetSelected(Flag);
}

void CCadArrow::AdjustRefernce(CPoint p)
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

CRect CCadArrow::GetRect(void)
{
	CRect r;
	int i;
	CPoint P3, P4,Points[3];
	CalcPoints(P3, P4, CPoint(0, 0), CScale(1.0, 1.0));
	Points[0] = GetP1();
	Points[1] = P3;
	Points[2] = P4;
	int MinX, MinY, MaxX, MaxY;
	MinX = MaxX = GetP1().x;
	MinY = MaxY = GetP1().y;
	for (i = 1; i < 3; ++i)
	{
		if (MinX > Points[i].x) MinX = Points[i].x;
		if (MinY > Points[i].y) MinY = Points[i].y;
		if (MaxX < Points[i].x) MaxX = Points[i].x;
		if (MaxY < Points[i].y) MaxY = Points[i].y;
	}
	r.SetRect(MinX, MinY, MaxX, MaxY);
	r.NormalizeRect();
	return r;
}

void CCadArrow::CalcPoints(CPoint & a, CPoint & b, CPoint Offset, CScale Scale)
{
	//-----------------------------------------------
	// CalcPoints
	//
	//	This function is used to calculate the
	// two other points that make up the triange
	// for the arrow.  We use m_P1 and m_P2 to do
	// this.
	//
	// parameters:
	//		a.....reference to the second ppoint
	//		b.....reference to the third point
	//		Offset....Reference Point
	//		Scale.....Scale factor
	//----------------------------------------------
	CSize diff;

	diff = GetP1() - GetP2();	//calculate difference

	if (diff.cy == 0)	//arrow pointing left or right
	{
		int dx = (diff.cx > 0)?m_Attrib.m_Len:-m_Attrib.m_Len;
		a = Scale * CPoint(GetP1().x - dx, GetP2().y + m_Attrib.m_ArrowWidth / 2) + Offset;
		b = Scale * CPoint(GetP1().x - dx, GetP2().y - m_Attrib.m_ArrowWidth / 2) + Offset;
	}
	else if (diff.cx == 0)	//arrow pointing up or down
	{
		int dy = (diff.cy > 0) ? m_Attrib.m_Len : -m_Attrib.m_Len;
		a = Scale * CPoint(GetP2().x + m_Attrib.m_ArrowWidth / 2, GetP1().y - dy) + Offset;
		b = Scale * CPoint(GetP2().x - m_Attrib.m_ArrowWidth / 2, GetP1().y - dy) + Offset;
	}
	else
	{
		double Angle;
		double x, y,dx,dy;
		//arrow pointing at some angle
		if (GetP1().x > GetP2().x && GetP1().y > GetP2().y)	//works
		{
			Angle =  - atan(double(GetP1().y - GetP2().y) / double(GetP1().x - GetP2().x));
			dx = m_Attrib.m_Len * cos(Angle);
			dy = m_Attrib.m_Len * sin(Angle);
			Angle += 3.14159 / 2.0;
			x = double(m_Attrib.m_ArrowWidth / 2) * cos(Angle);
			y = double(m_Attrib.m_ArrowWidth / 2) * sin(Angle);
			a = Scale * CPoint(GetP1().x - int(dx + x), GetP1().y + int(dy + y)) + Offset;
			b = Scale * CPoint(GetP1().x - int(dx - x), GetP1().y + int(dy - y)) + Offset;
		}
		else if (GetP1().x > GetP2().x && GetP1().y < GetP2().y)	//Works
		{
			Angle = - atan(double(GetP2().y - GetP1().y) / double(GetP1().x - GetP2().x));
			dx = m_Attrib.m_Len * cos(Angle);
			dy = m_Attrib.m_Len * sin(Angle);
			Angle += 3.14159 / 2.0;
			x = double(m_Attrib.m_ArrowWidth / 2) * cos(Angle);
			y = double(m_Attrib.m_ArrowWidth / 2) * sin(Angle);
			a = Scale * CPoint(GetP1().x - int(dx + x), GetP1().y - int(dy + y)) + Offset;
			b = Scale * CPoint(GetP1().x - int(dx - x), GetP1().y - int(dy - y)) + Offset;

		}
		else if (GetP1().x < GetP2().x && GetP1().y > GetP2().y)//works
		{
			Angle = - atan(double(GetP1().y - GetP2().y) / double(GetP1().x - GetP2().x));
			dx = m_Attrib.m_Len * cos(Angle);
			dy = m_Attrib.m_Len * sin(Angle);
			Angle += 3.14159 / 2.0;
			x = double(m_Attrib.m_ArrowWidth / 2) * cos(Angle);
			y = double(m_Attrib.m_ArrowWidth / 2) * sin(Angle);
			a = Scale * CPoint(GetP1().x + int(dx + x), GetP1().y - int(dy + y)) + Offset;
			b = Scale * CPoint(GetP1().x + int(dx - x), GetP1().y - int(dy - y)) + Offset;
		}
		else if (GetP1().x < GetP2().x && GetP1().y < GetP2().y)	//works
		{
			Angle = -atan(double(GetP1().y - GetP2().y) / double(GetP1().x - GetP2().x));
			dx = m_Attrib.m_Len * cos(Angle);
			dy = m_Attrib.m_Len * sin(Angle);
			Angle += 3.14159 / 2.0;
			x = double(m_Attrib.m_ArrowWidth / 2) * cos(Angle);
			y = double(m_Attrib.m_ArrowWidth / 2) * sin(Angle);
			a = Scale * CPoint(GetP1().x + int(dx + x), GetP1().y - int(dy + y)) + Offset;
			b = Scale * CPoint(GetP1().x + int(dx - x), GetP1().y - int(dy - y)) + Offset;

		}
	}
}



CCadArrow CCadArrow::operator=(CCadArrow & Ca)
{

	m_Attrib.m_Color = Ca.m_Attrib.m_Color;
	m_Attrib.m_Len = Ca.m_Attrib.m_Len;
	m_Attrib.m_ArrowWidth = Ca.m_Attrib.m_ArrowWidth;
	SetP1(Ca.GetP1());
	SetP2(Ca.GetP2());
	return *this;
}

void CCadArrow::RenderEnable(int e)
{
	CCadArrow::m_RenderEnable = e;
}

CPoint CCadArrow::GetCenter()
{
	CRect rect = this->GetRect();
	return rect.CenterPoint();
}

// Moves the center of the object to the spcified point
void CCadArrow::ChangeCenter(CSize p)
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


CSize CCadArrow::GetSize()
{
	CRect rect = GetRect();
	return rect.Size();
}


void CCadArrow::ChangeSize(CSize Sz)
{
	SetP2(GetP2() + Sz);
}
