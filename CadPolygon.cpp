// Polygon.cpp: implementation of the CCadPolygon class.
//
///////////////////////////////////////////////

#include "stdafx.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static int IDcount = 0;

///////////////////////////////////////////////
// Construction/Destruction
///////////////////////////////////////////////

CCadPolygon::CCadPolygon(): CCadObject(OBJECT_TYPE_POLY)
{
	m_pVertex = new CPoint[256];		//pointer to array of vertexes
	m_Count = 0;
	m_size = 256;
	m_PolyID = ++IDcount;
	m_MinX = m_MinY = m_MaxX = m_MaxY = 0;
	m_pPenLine = 0;
	m_InCount = 0;
	m_LineColor = RGB(0, 0, 0);
	m_Width = 0;
}

CCadPolygon::CCadPolygon(CCadPolygon &v) : CCadObject(OBJECT_TYPE_POLY)
{
	m_pVertex = new CPoint[256];		//pointer to array of vertexes
	m_Count = v.m_Count;
	m_size = 256;
	m_PolyID = ++IDcount;
	m_MinX = v.m_MinX;
	m_MinY = v.m_MinY;
	m_MaxX = v.m_MaxX;
	m_MaxY = v.m_MaxY;
	m_LineColor = v.m_LineColor;
	m_Width = v.m_Width;
	m_pPenLine = 0;
	m_InCount = 0;
	int i;
	for(i=0;i<m_Count;++i)
		m_pVertex[i] = v.m_pVertex[i];
}

CCadPolygon::CCadPolygon(int size) : CCadObject(OBJECT_TYPE_POLY)
{
	m_pVertex = new CPoint[size];		//pointer to array of vertexes
	m_Count = 0;
	m_size = size;
	m_PolyID = ++IDcount;
	m_MinX = m_MinY = m_MaxX = m_MaxY = 0;
	m_pPenLine = 0;
	m_InCount = 0;
	m_LineColor = RGB(0, 0, 0);
	m_Width = 0;
}

CCadPolygon::~CCadPolygon()
{
	delete[] m_pVertex;
	if(m_pPenLine) delete m_pPenLine;
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
		CPen *oldpen;
		int Lw;
		if ((Lw = int(Scale.m_ScaleX * m_Width)) < 1) Lw = 1;
		int i = 0;

		if ((GetLastMode() != mode) || GetDirty())
		{
			if (m_pPenLine) delete m_pPenLine;
			switch (mode)
			{
			case OBJECT_MODE_FINAL:
				m_pPenLine = new CPen(PS_SOLID, Lw, m_LineColor);
				break;
			case OBJECT_MODE_SELECTED:
				m_pPenLine = new CPen(PS_SOLID, Lw, RGB(200, 50, 50));
				break;
			case OBJECT_MODE_SKETCH:
				m_pPenLine = new CPen(PS_DOT, 1, m_LineColor);
				break;
			}
			SetDirty(0);
		}
		switch (mode)
		{
		case OBJECT_MODE_FINAL:
			oldpen = pDC->SelectObject(m_pPenLine);
			pDC->MoveTo(Scale * m_pVertex[i] + Offset);
			for (i = 1; i < m_Count; ++i)
				pDC->LineTo(Scale * m_pVertex[i] + Offset);
			pDC->LineTo(Scale * m_pVertex[0] + Offset);
			pDC->SelectObject(oldpen);
			SetLastMode(mode);
			break;
		case OBJECT_MODE_SELECTED:
			oldpen = pDC->SelectObject(m_pPenLine);
			pDC->MoveTo(Scale * m_pVertex[i] + Offset);
			for (i = 1; i < this->m_Count; ++i)
				pDC->LineTo(Scale * m_pVertex[i] + Offset);
			pDC->LineTo(Scale * m_pVertex[0] + Offset);
			for (i = 0; i < this->m_Count; ++i)
			{
				CPoint p1, p2, p;
				p = Scale * m_pVertex[i] + Offset;
				p1 = p + CPoint(4, 4);
				p2 = p - CPoint(4, 4);
				CRect rect;
				rect.SetRect(p2, p1);
				pDC->Rectangle(&rect);
			}
			pDC->SelectObject(oldpen);
			SetLastMode(mode);
			break;
		case OBJECT_MODE_SKETCH:
			oldpen = pDC->SelectObject(m_pPenLine);
			pDC->MoveTo(Scale * m_pVertex[i] + Offset);
			for (i = 1; i < this->m_Count; ++i)
				pDC->LineTo(Scale * m_pVertex[i] + Offset);
			SetLastMode(mode);
			break;
		case OBJECT_MODE_ERASE:
			break;
		}
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
	int   i, j=m_Count-1 ;
	BOOL  Enclosed=0      ;
	int Xintercept;
	CPoint *vert = new CPoint[m_Count];
	for(i=0;i<m_Count;++i)
		vert[i] = m_pVertex[i] + Off;
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
		for (i=0; i<m_Count; i++)
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

void CCadPolygon::AddPoint(CPoint nP)
{
	/*************************************
	** AddPoint
	**
	** Adds a new vertex to the polygon.
	**
	** parameters:
	**	nP....point of the new vertex.
	*************************************/
	m_pVertex[m_Count] = nP;
	//------------------------------------------
	// Create a rectangle that enbloses polygon
	//------------------------------------------
	if(m_Count == 0)
	{
		m_MinX = nP.x;
		m_MaxX = nP.x;
		m_MinY = nP.y;
		m_MaxY = nP.y;
	}
	else
	{
		if(nP.x > m_MaxX) m_MaxX = nP.x;
		else if (m_MinX > nP.x) m_MinX = nP.x;
		if(nP.y > m_MaxY) m_MaxY = nP.y;
		else if(m_MinY > nP.y) m_MinY = nP.y;
	}
	++m_Count;
}

int CCadPolygon::GetCount()
{
	/***********************************
	** GetCount
	**	Retturns the number of points
	** (verticies) in the polygon
	***********************************/
	return m_Count;
}

int CCadPolygon::DeleteLastPoint()
{
	/**********************************
	** DeleteLastPoint
	**	Deletes last point added to
	** polygon.
	** RETURNS:new vertex count.
	**********************************/
	if(m_Count > 0)
		--m_Count;
	return m_Count;
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
	if(nP == m_pVertex[m_Count - 1])
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
	return m_pVertex;
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
	this->SetPolySize(pP->GetPolySize());
	this->SetCount(pP->GetCount());
	m_MinX = pP->m_MinX;
	m_MinY = pP->m_MinY;
	m_MaxX = pP->m_MaxX;
	m_MaxY = pP->m_MaxY;

	for(i=0;i<m_Count;++i)
	{
		this->m_pVertex[i] = pPA[i];
	}
}

void CCadPolygon::Reset()
{
	m_Count = 0;
	SetSelected(0);	//initial not selected
}

CRect CCadPolygon::GetRect()
{
	CRect Rect;
	Rect.SetRect(m_MinX,m_MinY,m_MaxX,m_MaxY);
	Rect.NormalizeRect();
	return Rect;
}

void CCadPolygon::SetCurPoint(CPoint nP)
{
	m_pVertex[m_Count] = nP;
	++m_Count;
}

CCadPolygon CCadPolygon::operator=(CCadPolygon &v)
{
	this->m_pPenLine =0;
	m_size = v.m_size;
	m_Count = v.m_Count;
	m_Width = v.m_Width;
	m_LineColor = v.m_LineColor;
	m_MaxY = v.m_MaxY;
	m_MinY = v.m_MinY;
	m_MaxX = v.m_MaxX;
	m_MinX = v.m_MinX;
	m_PolyID = v.m_PolyID;
	for(int i=0;i<m_Count	;++i)
		m_pVertex[i] = v.m_pVertex[i];
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
	
	LookAHeadToken = pParser->Expect(TOKEN_POLY, LookAHeadToken, pIN);
	LookAHeadToken = pParser->Expect('(', LookAHeadToken, pIN);
	LookAHeadToken = PolyParams(pIN,LookAHeadToken, ppDrawing, pParser);
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
			LookAHeadToken = pParser->Color(TOKEN_LINE_COLOR, pIN, m_LineColor, LookAHeadToken);
			break;
		case TOKEN_LINE_WIDTH:
			LookAHeadToken = pParser->DecimalValue(TOKEN_LINE_WIDTH, pIN, m_Width, LookAHeadToken);
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
	LookAHeadToken = pParser->DecimalValue(TOKEN_VERTEX, pIN, m_Count, LookAHeadToken);
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
	LookAHeadToken = pParser->Point(TOKEN_VERTEX_POINT, pIN, m_pVertex[m_InCount++], LookAHeadToken);
	LookAHeadToken = VertexList1(pIN, LookAHeadToken, ppDrawing, pParser);
	return LookAHeadToken;
}

int CCadPolygon::VertexList1(FILE* pIN, int LookAHeadToken, CCadDrawing** ppDrawing, CFileParser* pParser)
{
	//--------------------------------------------------------
	// VertexList1	-> ',' VertexList
	//				-> .
	//--------------------------------------------------------
	switch (LookAHeadToken)
	{
	case ',':
		LookAHeadToken = pParser->Expect(',', LookAHeadToken, pIN);
		LookAHeadToken = VertexList(pIN, LookAHeadToken, ppDrawing, pParser);
		break;
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

	Indent1 = theApp.IndentString(Indent1, 256, Indent);
	Indent2 = theApp.IndentString(Indent2, 256, Indent + 4);
	Indent3 = theApp.IndentString(Indent3, 256, Indent + 8);

	fprintf(pO, "%s%s(%s,%s)\n%s{\n%s%s\n%s[\n",
		Indent1,
		CFileParser::TokenLookup(TOKEN_POLY),
		CFileParser::SaveColor(s3, 64, m_LineColor, TOKEN_LINE_COLOR),
		CFileParser::SaveDecimalValue(s4, 64, TOKEN_LINE_WIDTH, m_Width),
		Indent1,
		Indent2,
		CFileParser::SaveDecimalValue(s5, 64, TOKEN_VERTEX, m_Count),
		Indent2
	);
	for (int i = 0; i < m_Count; ++i)
	{
		fprintf(pO, "%s%s", 
			Indent3,
			CFileParser::SavePoint(s4,64,TOKEN_VERTEX_POINT, m_pVertex[i])
		);
		if (i < (m_Count - 1))
			fprintf(pO, ",\n");
		else
			fprintf(pO, "\n%s]\n%s}\n", Indent2,Indent1);
	}
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

	for(i=0,loop=1;(i<this->m_Count) && loop;++i)
	{
		p = m_pVertex[i];
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
	this->m_pVertex[Vi] = p;
	UpdateMinMax();
}

void CCadPolygon::Move(CPoint p)
{
	CPoint Diff;

	Diff = p - m_pVertex[0];
	int i;
	for(i=0;i<m_Count;++i)
		m_pVertex[i] += Diff;
	UpdateMinMax();
}

void CCadPolygon::UpdateMinMax()
{
	int i;
	m_MinX = 0;
	m_MaxX = 0;
	m_MinY = 0;
	m_MaxY = 0;
	for(i=0;i<m_Count;++i)
	{
		if(m_pVertex[i].x > m_MaxX) m_MaxX = m_pVertex[i].x;
		else if (m_MinX > m_pVertex[i].x) m_MinX = m_pVertex[i].x;
		if(m_pVertex[i].y > m_MaxY) m_MaxY = m_pVertex[i].y;
		else if(m_MinY > m_pVertex[i].y) m_MinY = m_pVertex[i].y;
	}
}

CPoint CCadPolygon::GetReference()
{
	return m_pVertex[0];
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
	for(i=0;i<m_Count;++i)
		m_pVertex[i] -= p;
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
	for (i = 0; i<m_Count; ++i)
		m_pVertex[i] -= p;
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
