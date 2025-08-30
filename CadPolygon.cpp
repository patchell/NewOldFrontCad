// Polygon.cpp: implementation of the CCadPolygon class.
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

CCadPolygon::CCadPolygon(): CCadObject(OBJECT_TYPE_POLY)
{
	m_MinX = m_MinY = m_MaxX = m_MaxY = 0;
}

CCadPolygon::CCadPolygon(CCadPolygon &v) : CCadObject(OBJECT_TYPE_POLY)
{
	GetAttributes()->m_pVertex = new CPoint[256];		//pointer to array of vertexes
	GetAttributes()->m_Count = v.GetAttributes()->m_Count;
	GetAttributes()->m_Size = v.GetAttributes()->m_Size;
	m_MinX = v.m_MinX;
	m_MinY = v.m_MinY;
	m_MaxX = v.m_MaxX;
	m_MaxY = v.m_MaxY;
	GetAttributes()->m_LineColor = v.GetAttributes()->m_LineColor;
	GetAttributes()->m_LineWidth = v.GetAttributes()->m_LineWidth;
	int i;
	for(i=0;i< GetAttributes()->m_Count;++i)
		GetAttributes()->m_pVertex[i] = v.GetAttributes()->m_pVertex[i];
}

CCadPolygon::~CCadPolygon()
{
}

void CCadPolygon::Draw(CDC *pDC,int mode,CPoint Offset,CScale Scale)
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
	if (CCadPolygon::m_RenderEnable)
	{
		CPen *oldpen = 0, penLine, penPoint;
		CBrush* oldbrush = 0, brushFill, brushPoint;
		int Lw;
		CPoint* pP = 0;
		int i = 0;

		if ((Lw = int(Scale.m_ScaleX * GetAttributes()->m_LineWidth)) < 1) Lw = 1;
		//-----------------------
		// need to scale the
		// verticies
		//-----------------------
		pP = new CPoint[GetAttributes()->m_Size];
		for (i = 0; i < GetAttributes()->m_Size; ++i)
		{
			pP[i] = Scale * GetAttributes()->m_pVertex[i] + Offset;
		}

		switch (mode)
		{
		case OBJECT_MODE_FINAL:
			penLine.CreatePen(PS_SOLID, Lw, GetAttributes()->m_LineColor);
			if(GetAttributes()->m_Transparent || CCadObject::AreShapeFillsDisabled())
				brushFill.CreateStockObject(HOLLOW_BRUSH);
			else
				brushFill.CreateSolidBrush(GetAttributes()->m_FillColor);
			break;
		case OBJECT_MODE_SELECTED:
			penLine.CreatePen(PS_SOLID, Lw, RGB(200, 50, 50));
			if (GetAttributes()->m_Transparent || CCadObject::AreShapeFillsDisabled())
				brushFill.CreateStockObject(HOLLOW_BRUSH);
			else
				brushFill.CreateSolidBrush(GetAttributes()->m_FillColor);
			break;
		case OBJECT_MODE_SKETCH:
			penLine.CreatePen(PS_DOT, 1, GetAttributes()->m_LineColor);
			brushFill.CreateStockObject(HOLLOW_BRUSH);
			break;
		}
		brushPoint.CreateSolidBrush(RGB(0, 0, 255));
		penPoint.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
		oldpen = pDC->SelectObject(&penLine);
		oldbrush = pDC->SelectObject(&brushFill);
		//-------------------------------
		// Draw the polygon
		//-------------------------------
		switch (mode)
		{
		case OBJECT_MODE_FINAL:
			pDC->Polygon(pP, GetAttributes()->m_Size);
			break;
		case OBJECT_MODE_SELECTED:
			i = 0;
			pDC->Polygon(pP, GetAttributes()->m_Size);
			pDC->SelectObject(&penPoint);
			pDC->SelectObject(&brushPoint);	
			for (i = 0; i < GetAttributes()->m_Size; ++i)
			{
				CPoint p1, p2, p;
				p = pP[i];
				p1 = p + CPoint(4, 4);
				p2 = p - CPoint(4, 4);
				CRect rect;
				rect.SetRect(p2, p1);
				pDC->Rectangle(&rect);
			}
			break;
		case OBJECT_MODE_SKETCH:
			pDC->Polygon(pP, GetAttributes()->m_Size);
			pDC->SelectObject(oldpen);
			pDC->SelectObject(&penPoint);
			pDC->SelectObject(&brushPoint);
			for (i = 0; i < GetAttributes()->m_Size; ++i)
			{
				CPoint p1, p2, p;
				p = pP[i];
				p1 = p + CPoint(4, 4);
				p2 = p - CPoint(4, 4);
				CRect rect;
				rect.SetRect(p2, p1);
				pDC->Rectangle(&rect);
			}
			break;
		case OBJECT_MODE_ERASE:
			break;
		}
		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
	}

}

void CCadPolygon::Create(CPoint *)
{

}

BOOL CCadPolygon::PointEnclosed(CPoint nP,CSize Off)
{
	/*****************************************
	** PointEnclosed
	**	This function determines if a point
	** is enclosed within a polygon.
	**
	** parameters:
	**	nP....point to test
	** Returns: TRUE if point inside
	**          FALSE if point is outside
	*****************************************/
	int   i, j= GetAttributes()->m_Count-1 ;
	BOOL  Enclosed=0      ;
	int Xintercept;
	CPoint *vert = new CPoint[GetAttributes()->m_Count];
	for(i=0;i< GetAttributes()->m_Count;++i)
		vert[i] = GetAttributes()->m_pVertex[i] + Off;
	//--------------------------------------------
	// Do a precheck agains the rectangle that
	// encloses the polygon
	//--------------------------------------------
	if(m_MinX < nP.x && m_MaxX > nP.x && m_MinY < nP.y && m_MaxY > nP.y)
	{
		//----------------------------------
		// very good chance now that point
		// is in polygon, so make a
		// thorough check
		//---------------------------------
		for (i=0; i< GetAttributes()->m_Count; i++)
		{
			if (( (vert[i].y< nP.y && vert[j].y>=nP.y)
				|| (vert[j].y< nP.y && vert[i].y>=nP.y))
				&&  (vert[i].x<=nP.x || vert[j].x<=nP.x) )
			{
				Xintercept = vert[i].x+((nP.y-vert[i].y)*(vert[j].x-vert[i].x))/(vert[j].y-vert[i].y);
				Enclosed ^= Xintercept < nP.x;
			}
			j=i;
		}
	}
	delete[] vert;
	return Enclosed;
}

BOOL CCadPolygon::AddPoint(CPoint ptNewPoint, BOOL bInc, BOOL bIncSize)
{
	//-------------------------------------
	// AddPoint
	//
	// Adds a new vertex to the polygon.
	//
	// parameters:
	//	nP....point of the new vertex.
	//-------------------------------------

	//------------------------------------------
	// Create a rectangle that enbloses polygon
	//------------------------------------------
	if(GetAttributes()->m_Count == 0)
	{
		m_MinX = ptNewPoint.x;
		m_MaxX = ptNewPoint.x;
		m_MinY = ptNewPoint.y;
		m_MaxY = ptNewPoint.y;
	}
	else
	{
		if(ptNewPoint.x > m_MaxX) m_MaxX = ptNewPoint.x;
		else if (m_MinX > ptNewPoint.x) m_MinX = ptNewPoint.x;
		if(ptNewPoint.y > m_MaxY) m_MaxY = ptNewPoint.y;
		else if(m_MinY > ptNewPoint.y) m_MinY = ptNewPoint.y;
	}
	return GetAttributes()->AddPoint(ptNewPoint, bInc, bIncSize);
}

int CCadPolygon::GetCount()
{
	/***********************************
	** GetCount
	**	Retturns the number of points
	** (verticies) in the polygon
	***********************************/
	return GetAttributes()->m_Count;
}

int CCadPolygon::DeleteLastPoint()
{
	/**********************************
	** DeleteLastPoint
	**	Deletes last point added to
	** polygon.
	** RETURNS:new vertex count.
	**********************************/
	if(GetAttributes()->m_Count > 0)
		--GetAttributes()->m_Count;
	return GetAttributes()->m_Count;
}

int CCadPolygon::CompareToLast(CPoint nP)
{
	/*********************************
	** CompareToLast
	**	This function compares a point
	** to the last point that was added
	**to a polygon.
	**
	** parameter:
	**	nP.....point to compare
	**
	** REtuns: TRUE if equal
	**	       FALSE if not equal.
	*********************************/
	int rV = 0;
	if(nP == GetAttributes()->m_pVertex[GetAttributes()->m_Count - 1])
		rV = 1;
	return rV;
}

CPoint * CCadPolygon::GetPoints()
{
	/*************************************
	** GetPoints
	**
	** Returns the list of points that
	** define the polygon.
	*************************************/
	return GetAttributes()->m_pVertex;
}

void CCadPolygon::Copy(CCadPolygon *pP)
{
	/************************************
	** Copy
	**
	** This function is used to copy
	** another polygon into this polygon
	**
	** parameter:
	**	pP.....pointer of polygon to copy
	************************************/
	int i;
	CPoint *pPA = pP->GetPoints();
	SetPolySize(pP->GetPolySize());
	SetCount(pP->GetCount());
	m_MinX = pP->m_MinX;
	m_MinY = pP->m_MinY;
	m_MaxX = pP->m_MaxX;
	m_MaxY = pP->m_MaxY;

	for(i=0;i< GetAttributes()->m_Count;++i)
	{
		GetAttributes()->m_pVertex[i] = pPA[i];
	}
}

void CCadPolygon::Reset()
{
	GetAttributes()->m_Count = 0;
	SetSelected(0);	//initial not selected
}

CRect CCadPolygon::GetRect()
{
	CRect Rect;
	Rect.SetRect(m_MinX,m_MinY,m_MaxX,m_MaxY);
	Rect.NormalizeRect();
	return Rect;
}

void CCadPolygon::SetCurPoint(CPoint newPoint)
{
	GetAttributes()->m_pVertex[GetAttributes()->m_Count] = newPoint;
}

CCadPolygon CCadPolygon::operator=(CCadPolygon &v)
{
	GetAttributes()->m_Size = v.GetAttributes()->m_Size;
	GetAttributes()->m_Count = v.GetAttributes()->m_Count;
	GetAttributes()->m_LineWidth = v.GetAttributes()->m_LineWidth;
	GetAttributes()->m_LineColor = v.GetAttributes()->m_LineColor;
	m_MaxY = v.m_MaxY;
	m_MinY = v.m_MinY;
	m_MaxX = v.m_MaxX;
	m_MinX = v.m_MinX;
	for(int i=0;i< GetAttributes()->m_Count	;++i)
		GetAttributes()->m_pVertex[i] = v.GetAttributes()->m_pVertex[i];
	return *this;
}

int CCadPolygon::CheckSelected(CPoint p,CSize Offset)
{
	return this->PointEnclosed(p,Offset);
}

int CCadPolygon::Parse(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser)
{
	//--------------------------------------------------------
	// Polygon		-> POLY '(' PolyParams ')' '{' Vertex '}';
	//--------------------------------------------------------
	BOOL Loop = 1;

	switch (LookAHeadToken)
	{
	case TOKEN_POLY:
		LookAHeadToken = pParser->Expect(TOKEN_POLY, LookAHeadToken, pIN);
		break;
	case TOKEN_POLYFILL:
		LookAHeadToken = pParser->Expect(TOKEN_POLYFILL, LookAHeadToken, pIN);
		break;
	default:
		break;
	}
	LookAHeadToken = pParser->Expect('(', LookAHeadToken, pIN);
	LookAHeadToken = PolyParams(pIN, LookAHeadToken, ppDrawing, pParser);
	LookAHeadToken = pParser->Expect(')', LookAHeadToken, pIN);
	LookAHeadToken = pParser->Expect('{', LookAHeadToken, pIN);
	LookAHeadToken = Vertex(pIN, LookAHeadToken, ppDrawing, pParser);
	LookAHeadToken = pParser->Expect('}', LookAHeadToken, pIN);

	(*ppDrawing)->AddObject(this);
	return LookAHeadToken;
}

int CCadPolygon::PolyParams(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser)
{
	//--------------------------------------------------------
	// PolyParams	-> LINECOLLOR '(' COLOR ')'
	//				-> LINEWIDTH '(' NUMBER ')'
	//				-> FILL_COLOR '(' COLOR ')'
	//				-> TRANSPARENT
	//				-> .
	//--------------------------------------------------------
	BOOL Loop = 1;

	while (Loop)
	{
		switch (LookAHeadToken)
		{
		case ',':
			LookAHeadToken = pParser->Expect(',',LookAHeadToken, pIN);
			break;
		case TOKEN_LINE_COLOR:
			LookAHeadToken = pParser->Color(TOKEN_LINE_COLOR, pIN, GetAttributes()->m_LineColor, LookAHeadToken);
			break;
		case TOKEN_LINE_WIDTH:
			LookAHeadToken = pParser->DecimalValue(TOKEN_LINE_WIDTH, pIN, GetAttributes()->m_LineWidth, LookAHeadToken);
			break;
		case TOKEN_FILL_COLOR:
			LookAHeadToken = pParser->Color(TOKEN_FILL_COLOR, pIN, GetAttributes()->m_FillColor, LookAHeadToken);
			break;
		case TOKEN_TRANSPARENT:
			LookAHeadToken = pParser->DecimalValue(TOKEN_TRANSPARENT, pIN, GetAttributes()->m_Transparent, LookAHeadToken);
			break;
		case ')':
			Loop = FALSE;
			break;
		default:
			Loop = FALSE;
			break;
		}
	}
	return LookAHeadToken;
}

int CCadPolygon::Vertex(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser)
{
	//--------------------------------------------------------
	// Vertex		-> VERTEX '(' NUMBEER ')' '[' VertexList ']';
	//--------------------------------------------------------
	LookAHeadToken = pParser->DecimalValue(TOKEN_VERTEX, pIN, GetAttributes()->m_Size,LookAHeadToken);
	LookAHeadToken = pParser->Expect('[', LookAHeadToken, pIN);
	LookAHeadToken = VertexList(pIN, LookAHeadToken, ppDrawing, pParser);
	LookAHeadToken = pParser->Expect(']', LookAHeadToken, pIN);
	return LookAHeadToken;
}

int CCadPolygon::VertexList(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser)
{
	//--------------------------------------------------------
	// VertexList	-> VERTEX_POINT '(' NUMBER ',' NUMBER ')' VertexList1;
	//--------------------------------------------------------
	BOOL Loop = TRUE;
	CPoint ptTemp;

	while (Loop)
	{
		switch (LookAHeadToken)
		{
		case TOKEN_VERTEX_POINT:
			LookAHeadToken = pParser->Point(TOKEN_VERTEX_POINT, pIN, ptTemp, LookAHeadToken);
			AddPoint(ptTemp,TRUE, FALSE);
			break;
		case ',':
			LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
			break;
		default:
			Loop = FALSE;
			break;
		}
	}
	return LookAHeadToken;
}

void CCadPolygon::Save(FILE* pO, int Indent)
{
	char* Indent1 = new char[256];
	char* Indent2 = new char[256];
	char* Indent3 = new char[256];
	char* s3 = new char[64];
	char* s4 = new char[64];
	char* s5 = new char[64];
	char* s6 = new char[64];
	char* s7 = new char[64];

	Indent1 = theApp.IndentString(Indent1, 256, Indent);
	Indent2 = theApp.IndentString(Indent2, 256, Indent + 4);
	Indent3 = theApp.IndentString(Indent3, 256, Indent + 8);

	fprintf(pO, "%s%s(%s,%s,%s,%s)\n%s{\n%s%s\n%s[\n",
		Indent1,
		CFileParser::TokenLookup(TOKEN_POLY),
		CFileParser::SaveColor(s3, 64, GetAttributes()->m_LineColor, TOKEN_LINE_COLOR),
		CFileParser::SaveDecimalValue(s4, 64, TOKEN_LINE_WIDTH, GetAttributes()->m_LineWidth),
		CFileParser::SaveColor(s6, 64, GetAttributes()->m_FillColor, TOKEN_FILL_COLOR),
		CFileParser::SaveDecimalValue(s7, 64, TOKEN_TRANSPARENT, GetAttributes()->m_Transparent),
		Indent1,
		Indent2,
		CFileParser::SaveDecimalValue(s5, 64, TOKEN_VERTEX, GetAttributes()->m_Size),
		Indent2
	);
	for (int i = 0; i < GetAttributes()->m_Size; ++i)
	{
		fprintf(pO, "%s%s", 
			Indent3,
			CFileParser::SavePoint(s4,64,TOKEN_VERTEX_POINT, GetAttributes()->m_pVertex[i])
		);
		if (i < (GetAttributes()->m_Size - 1))
			fprintf(pO, ",\n");
		else
			fprintf(pO, "\n%s]\n%s}\n", Indent2,Indent1);
	}
	delete[]s7;
	delete[]s6;
	delete[]s5;
	delete[]s4;
	delete[]s3;
	delete[]Indent2;
	delete[]Indent1;
}

int CCadPolygon::GrabVertex(CPoint point)
{
	int i,loop,rV = -1;;
	CRect rect;
	CPoint p1,p2,p;

	for(i=0,loop=1;(i<this->GetAttributes()->m_Count) && loop;++i)
	{
		p = GetAttributes()->m_pVertex[i];
		p1 = p + CPoint(-4,-4);
		p2 = p + CPoint(4,4);
		rect.SetRect(p1,p2);
		rect.NormalizeRect();
		if(rect.PtInRect(point))
		{
			rV = i;
			loop = 0;
		}
	}
	return rV;
}

void CCadPolygon::SetVertex(int Vi, CPoint p)
{
	GetAttributes()->m_pVertex[Vi] = p;
	UpdateMinMax();
}

void CCadPolygon::Move(CPoint p)
{
	CPoint Diff;

	Diff = p - GetAttributes()->m_pVertex[0];
	int i;
	for(i=0;i< GetAttributes()->m_Count;++i)
		GetAttributes()->m_pVertex[i] += Diff;
	UpdateMinMax();
}

void CCadPolygon::UpdateMinMax()
{
	int i;
	m_MinX = 0;
	m_MaxX = 0;
	m_MinY = 0;
	m_MaxY = 0;
	for(i=0;i< GetAttributes()->m_Count;++i)
	{
		if(GetAttributes()->m_pVertex[i].x > m_MaxX) m_MaxX = GetAttributes()->m_pVertex[i].x;
		else if (m_MinX > GetAttributes()->m_pVertex[i].x) m_MinX = GetAttributes()->m_pVertex[i].x;
		if(GetAttributes()->m_pVertex[i].y > m_MaxY) m_MaxY = GetAttributes()->m_pVertex[i].y;
		else if(m_MinY > GetAttributes()->m_pVertex[i].y) m_MinY = GetAttributes()->m_pVertex[i].y;
	}
}

CPoint CCadPolygon::GetReference()
{
	return GetAttributes()->m_pVertex[0];
}

void CCadPolygon::AdjustRefernce(CPoint p)
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
	int i;
	for(i=0;i< GetAttributes()->m_Count;++i)
		GetAttributes()->m_pVertex[i] -= p;
	UpdateMinMax();
}

void CCadPolygon::RenderEnable(int e)
{
	CCadPolygon::m_RenderEnable = e;
}

CPoint CCadPolygon::GetCenter()
{
	CRect rect = this->GetRect();
	return rect.CenterPoint();
}

// Moves the center of the object to the spcified point
void CCadPolygon::ChangeCenter(CSize p)
{
	SetP1(GetP1() - p);
	int i;
	for (i = 0; i< GetAttributes()->m_Count; ++i)
		GetAttributes()->m_pVertex[i] -= p;
	UpdateMinMax();

}


CSize CCadPolygon::GetSize()
{
	CRect rect = GetRect();
	return rect.Size();
}


void CCadPolygon::ChangeSize(CSize Sz)
{
}

BOOL PolyAttributes::AddPoint(CPoint p, BOOL bInc, BOOL bIncSizeToo)
{
	BOOL rV = TRUE;
	int c, s;

	c = m_Count;
	s = m_Size;
	if (m_Count >= POLY_MAX_VERTECIES)
		rV = FALSE;
	else
	{
		if(bInc)
			m_pVertex[m_Count++] = p;
		else
			m_pVertex[m_Count] = p;
		if(bIncSizeToo)
			++m_Size;

	}
	return rV;
}