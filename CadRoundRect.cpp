// CadRoundRect.cpp: implementation of the CCadRoundRect class.
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

CCadRoundRect::CCadRoundRect():CCadObject(OBJECT_TYPE_RNDRECT)
{
	m_FillColor = RGB(0,0,0);
	m_LineColor = RGB(0, 0, 0);
	m_LineWidth = 1;
}

CCadRoundRect::CCadRoundRect(CCadRoundRect &r):CCadObject(OBJECT_TYPE_RNDRECT)
{
	m_FillColor = r.m_FillColor;
	m_LineColor = r.m_LineColor;
	m_LineWidth = r.m_LineWidth;
	SetP1(r.GetP1());
	SetP2(r.GetP2());
	m_P3 = r.m_P3;

}

CCadRoundRect::~CCadRoundRect()
{
}

void CCadRoundRect::Draw(CDC *pDC, int mode,CPoint Offset,CScale Scale)
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
	CPen *pOld = 0, penLine, penPoint;
	CBrush *pOldBr = 0, brushFill, brushSelect, brushPoint;
	CPoint P1,P2,P3,pointP3;
	CRect rect;
	CSize rectLWcomp;
	CSize Diff;

	int Lw;

	if (CCadRoundRect::m_RenderEnable)
	{
		P1 = Scale * GetP1() + Offset;
		P2 = Scale * GetP2() + Offset;
		P3 = Scale * m_P3;
		Lw = int(m_LineWidth * Scale.m_ScaleX);
		if (Lw <= 1 || OBJECT_MODE_SKETCH == mode)
		{
			Lw = 1;
			rectLWcomp = CSize(0, 0);
		}
		else
			rectLWcomp = CSize(Lw / 2, Lw / 2);
		//	rect.SetRect(P1,P2);
		switch (mode)
		{
		case OBJECT_MODE_FINAL:
			penLine.CreatePen(PS_SOLID, Lw, m_LineColor);
			if (CCadObject::AreShapeFillsDisabled())
				brushFill.CreateStockObject(NULL_BRUSH);
			else
				brushFill.CreateSolidBrush(m_FillColor);
			break;
		case OBJECT_MODE_SELECTED:
			penLine.CreatePen(PS_SOLID, Lw, RGB(200, 50, 50));
			penPoint.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
			brushSelect.CreateSolidBrush(RGB(200, 200, 255));
			brushPoint.CreateSolidBrush(RGB(0, 0, 255));
			if (CCadObject::AreShapeFillsDisabled())
				brushFill.CreateStockObject(NULL_BRUSH);
			else
				brushFill.CreateSolidBrush(RGB(100, 50, 50));
			break;
		case OBJECT_MODE_SKETCH:
			penLine.CreatePen(PS_DOT, 1, m_LineColor);
			penPoint.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
			brushSelect.CreateSolidBrush(RGB(0, 0, 255));
			brushFill.CreateStockObject(NULL_BRUSH);
			brushPoint.CreateSolidBrush(RGB(0, 0, 255));
			break;
		}
		SetRect(rect, P1, P2, rectLWcomp);
		pOld = pDC->SelectObject(&penLine);	
		pOldBr = pDC->SelectObject(&brushFill);
		switch (mode)
		{
		case OBJECT_MODE_FINAL:
			pDC->RoundRect(rect, P3);
			break;
		case OBJECT_MODE_SELECTED:
		{
			pDC->SelectObject(&brushSelect);
			pDC->RoundRect(rect, P3);
			Diff = CSize(4, 4);
			rect.SetRect(P1 - Diff, P1 + Diff);
			pDC->SelectObject(&penPoint);
			pDC->SelectObject(&brushPoint);
			pDC->Rectangle(&rect);
			rect.SetRect(P2 - Diff, P2 + Diff);
			pDC->Rectangle(&rect);
			pointP3 = P1 + P3;
			rect.SetRect(pointP3 - Diff, pointP3 + Diff);
			pDC->Rectangle(&rect);
			pointP3 = P2 - P3;
			rect.SetRect(pointP3 - Diff, pointP3 + Diff);
			pDC->Rectangle(&rect);
		}
		break;
		case OBJECT_MODE_SKETCH:
			pDC->RoundRect(rect, P3);
			Diff = CSize(4, 4);
			rect.SetRect(P1 - Diff, P1 + Diff);
			pDC->SelectObject(&penPoint);
			pDC->SelectObject(&brushPoint);
			pDC->Rectangle(&rect);
			rect.SetRect(P2 - Diff, P2 + Diff);
			pDC->Rectangle(&rect);
			pointP3 = P1 + P3;
			rect.SetRect(pointP3 - Diff, pointP3 + Diff);
			pDC->Rectangle(&rect);
			pointP3 = P2 - P3;
			rect.SetRect(pointP3 - Diff, pointP3 + Diff);
			pDC->Rectangle(&rect);
			break;
		case OBJECT_MODE_ERASE:
			break;
		}
		pDC->SelectObject(pOldBr);
		pDC->SelectObject(pOld);
	}
}

int CCadRoundRect::CheckSelected(CPoint p,CSize O)
{
	//------------------------------------------
	// CheckSelected
	//		This function checks to seeif the
	// given point is withing the object.
	//
	// parameters:
	//		p......point of interest
	//------------------------------------------
	CRect rect;
	int rV;
	CPoint P1 = GetP1() + O;
	CPoint P2 = GetP2() + O;
	rect.SetRect(P1,P2);
	rect.NormalizeRect();
	rV = (int)rect.PtInRect(p);
	return rV;
}

CCadRoundRect CCadRoundRect::operator=(CCadRoundRect &v)
{
	//----------------------------------------
	// operator=
	//		this function is used when the assing
	//operator is used to copy this object.
	//
	// parameters:
	//		v.....reference to object to copy
	//----------------------------------------
	SetP1(v.GetP1());
	SetP2(v.GetP2());
	m_P3 = v.m_P3;
	m_FillColor = v.m_FillColor;
	m_LineColor = v.m_LineColor;
	m_LineWidth = v.m_LineWidth;
	return *this;
}

void CCadRoundRect::Move(CPoint p)
{
	//----------------------------------------
	//	Move
	//		This function is used to move the
	// object.
	//
	// parameters:
	//		p.....point to move object to
	//----------------------------------------

	CSize Diff = p - GetP1();
	SetP1(GetP1() + Diff);
	SetP2(GetP2() + Diff);
}

int CCadRoundRect::Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser)
{
	LookAHeadToken = pParser->Expect(TOKEN_RNDRECT, LookAHeadToken, pIN);
	LookAHeadToken = pParser->Expect('(', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Point(TOKEN_POINT_1, pIN, GetP1(), LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Point(TOKEN_POINT_2, pIN, GetP2(), LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Point(TOKEN_POINT_3, pIN, m_P3, LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Color(TOKEN_LINE_COLOR, pIN, m_LineColor, LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Color(TOKEN_FILL_COLOR, pIN, m_FillColor, LookAHeadToken);
	LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
	LookAHeadToken = pParser->DecimalValue(TOKEN_LINE_WIDTH, pIN, m_LineWidth, LookAHeadToken);
	LookAHeadToken = pParser->Expect(')', LookAHeadToken, pIN);
	(*ppDrawing)->AddObject(this);
	return LookAHeadToken;
}

void CCadRoundRect::Save(FILE* pO, int Indent)
{
	//----------------------------------------
	// Save
	//		This function is used to save this
	// this object to a file.  Used by save
	// save file and save library.
	//
	// parameters:
	//		pO....pointer to file to save to
	//---------------------------------------
	char* s = new char[256];
	char* s1 = new char[64];
	char* s2 = new char[64];
	char* s3 = new char[64];
	char* s4 = new char[64];
	char* s5 = new char[64];
	char* s6 = new char[64];

	fprintf(pO, "%sRNDRECT(%s,%s,%s,%s,%s,%s)\n",
		theApp.IndentString(s, 256, Indent),
		CFileParser::SavePoint(s1, 64, TOKEN_POINT_1, GetP1()),
		CFileParser::SavePoint(s2, 64, TOKEN_POINT_2, GetP2()),
		CFileParser::SavePoint(s3, 64, TOKEN_POINT_3, m_P3),
		CFileParser::SaveColor(s4, 64, m_LineColor, TOKEN_LINE_COLOR),
		CFileParser::SaveColor(s5, 64, m_FillColor, TOKEN_FILL_COLOR),
		CFileParser::SaveDecimalValue(s6,64, TOKEN_LINE_WIDTH,m_LineWidth)
	);
	delete[]s6;
	delete[]s5;
	delete[]s4;
	delete[]s3;
	delete[]s2;
	delete[]s1;
	delete[]s;
}

int CCadRoundRect::GrabVertex(CPoint point)
{
	//----------------------------------------
	// Grab Vertex
	//		When the object is selected, there
	// are three different verticies that can
	// be grabbed to change the shape of the
	// object.
	//
	//	parameters:
	//		point......point where moused is at
	//		S..........Scale factor
	//	returns:
	//		index to the vertex if point was over a vertex
	//		-1 if no vertex was selected.
	//----------------------------------------
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
		else
		{
			CPoint p = GetP1() + m_P3;
			rect.SetRect(p+Diff,p-Diff);
			rect.NormalizeRect();
			if(rect.PtInRect(point))
				rV = 2;
			else
			{
				CPoint p = GetP2() - m_P3;
				rect.SetRect(p+Diff,p-Diff);
				rect.NormalizeRect();
				if(rect.PtInRect(point))
					rV = 3;
			}
		}
	}

	return rV;
}

void CCadRoundRect::SetVertex(int Vi, CPoint p)
{
	//----------------------------------------
	// SetVertex
	//		This function is used to change the
	// value of the specified vertex.
	//
	//	parameter:
	//		Vi.....Vertex Index
	//		p......New value of vertex
	//----------------------------------------

	CSize Diff;
	switch(Vi)
	{
		case 0:
			Diff = p-GetP1();
			SetP1(GetP1() + Diff);
			break;
		case 1:
			Diff = p-GetP2();
			SetP2(GetP2() + Diff);
			break;
		case 2:
			Diff = p - (GetP1() + m_P3);
			m_P3 += Diff;
			break;
		case 3:
			Diff = p - (GetP2() - m_P3);
			m_P3 += Diff;
			break;
	}
}

CPoint CCadRoundRect::GetReference()
{
	return GetP1();
}

void CCadRoundRect::AdjustRefernce(CPoint p)
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

void CCadRoundRect::RenderEnable(int e)
{
	CCadRoundRect::m_RenderEnable = e;
}

CPoint CCadRoundRect::GetCenter()
{
	CRect rect = this->GetRect();
	return rect.CenterPoint();
}

// Moves the center of the object to the spcified point
void CCadRoundRect::ChangeCenter(CSize p)
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


CSize CCadRoundRect::GetSize()
{
	CRect rect = GetRect();
	return rect.Size();
}


void CCadRoundRect::ChangeSize(CSize Sz)
{
	SetP2(GetP2() - Sz);
}
