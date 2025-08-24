// FileParser.cpp: implementation of the CFileParser class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FileParser.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static KEYWORD KeyWords[] = {
	{"ARC",TOKEN_ARC},
	{"ARCCENT",TOKEN_ARC_CENTER},
	{"CIRCLE",TOKEN_CIRCLE},
	{"ELLIPSE", TOKEN_ELLIPSE},
	{"RECT",TOKEN_RECT},
	{"RNDRECT",TOKEN_RNDRECT},
	{"POLY",TOKEN_POLY},
	{"POLYFILL",TOKEN_POLYFILL},
	{"LINE",TOKEN_LINE},
	{"HOLERND",TOKEN_HOLERND},
	{"HOLERECT",TOKEN_HOLERECT},
	{"HOLE1FLAT",TOKEN_HOLE_1FLAT},
	{"HOLE2FLAT",TOKEN_HOLE_2FLAT},
	{"LIBPART",TOKEN_LIBPART},
	{"TEXT",TOKEN_TEXT},
	{"POINT_1",TOKEN_POINT_1},
	{"POINT_2",TOKEN_POINT_2},
	{"POINT_3",TOKEN_POINT_3},
	{"VERTEX_POINT",TOKEN_VERTEX_POINT},
	{"BACKGROUND_COLOR",TOKEN_BACKGROUND_COLOR},
	{"LINE_COLOR",TOKEN_LINE_COLOR},
	{"TEXT_COLOR",TOKEN_TEXT_COLOR},
	{"FILL_COLOR",TOKEN_FILL_COLOR},
	{"RED",TOKEN_RED},
	{"GREEN",TOKEN_GREEN},
	{"BLUE",TOKEN_BLUE},
	{"COLOR",TOKEN_COLOR},
	{"FONT",TOKEN_FONT},
	{"WEIGHT",TOKEN_WEIGHT},
	{"FONTHIEGHT",TOKEN_FONTHIEGHT},
	{"FONTWIDTH",TOKEN_FONTWIDTH},
	{"RADIUS",TOKEN_RADIUS},
	{"FLATDIST",TOKEN_FLATDIST},
	{"DRAWFILE",TOKEN_DRAWFILE},
	{"LIBRARY",TOKEN_LIBFILE},
	{"VERTEX",TOKEN_VERTEX},
	{"RECTWIDTH",TOKEN_RECTWIDTH},
	{"RECTHIEGHT",TOKEN_RECTHIEGHT},
	{"ANGLE",TOKEN_ANGLE},
	{"TRANSPARENT",TOKEN_TRANSPARENT},
	{"REF",TOKEN_REFERENCE},
	{"BITMAP",TOKEN_BITMAP},
	{"FILE",TOKEN_FILE},
	{"ARROW",TOKEN_ARROW},
	{"LENGTH",TOKEN_LENGTH},
	{"ORIGIN",TOKEN_ORG},
	{"DIMENSION",TOKEN_DIMENSION},
	{"PRINTRECT",TOKEN_PRINTRECT },
	{"SIZE",TOKEN_SIZE },
	{"SCALE", TOKEN_SCALE},
	{"START",TOKEN_START_POINT},
	{"END",TOKEN_END_POINT},
	{"LINE_WIDTH",TOKEN_LINE_WIDTH},
	{"ARROW_LENGTH",TOKEN_ARROW_LENGTH},
	{"ARROW_WIDTH",TOKEN_ARROW_WIDTH},
	{"TRANSPARENT", TOKEN_TRANSPARENT},
	{NULL,0}
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileParser::CFileParser(CFrontCadDoc *pDoc)
{
	m_pDoc = pDoc;
	m_pLib = 0;
	m_pLob = 0;
	m_pDLob = 0;
	UnGetBuff = 0;
	m_LexIndex = 0;
	m_LexValue = 0;
	memset(m_LexBuff, 0, 256);
	UnGetBuff = 0;
	m_Col = 0;
	m_Line = 1;
}

CFileParser::CFileParser()
{
	m_pDLob = 0;
	m_pDoc = 0;
	m_pLib = 0;
	m_pLob = 0;
	UnGetBuff = 0;
	m_LexValue = 0;
	m_LexIndex = 0;
	memset(m_LexBuff, 0, 256);
	UnGetBuff = 0;
	m_Col = 0;
	m_Line = 1;
}

CFileParser::~CFileParser()
{

}

int CFileParser::GetChar(FILE *in)
{
	int rV;

	if(UnGetBuff)
	{
		rV = UnGetBuff;
		switch (rV)
		{
		case '\n':
			m_Line++;
			m_Col = 0;
			break;
		default:
			m_Col++;
			break;
		}
		UnGetBuff = 0;
	}
	else
	{
		rV = fgetc(in);
		switch (rV)
		{
		case '\n':
			m_Line++;
			m_Col = 0;
			break;
		default:
			m_Col++;
			break;
		}
	}
	return rV;
}

int CFileParser::UnGetChar(int c)
{
	UnGetBuff = c;
	return 0;
}

int CFileParser::Open(char *pFileName, CFrontCadDoc *pDoc)
{
	return 0;
}

int CFileParser::LookUp(char *pKW)
{
	int rV=-1;
	int i,loop;
	for(i=0,loop=1;KeyWords[i].KeyWord && loop;++i)
	{
		if(strcmp(KeyWords[i].KeyWord,pKW) == 0)
		{
			loop = 0;
			rV = KeyWords[i].Token;
		}
	}
	return rV;
}

const char* CFileParser::TokenLookup(int Token)
{
	const char* rV = NULL;
	int i;
	BOOL Loop = TRUE;

	for (i = 0; KeyWords[i].KeyWord && Loop; ++i)
	{
		if (KeyWords[i].Token == Token)
		{
			rV = KeyWords[i].KeyWord;
			Loop = FALSE;
		}
	}
	if (rV == NULL)
		rV = "Unknown Token";
	return rV;
}

int CFileParser::Expect(int ExpectedToken, int LookAHead, FILE *pIN)
{
	/********************************************
	** Expect
	**
	** this function is used to match
	** the expected token with the current
	** token.  If they match, get the
	** next token.
	**
	** parameter:
	**	pI.....input file stream
	**	token..expected token
	**
	** Returns:Next token, or -1 on error
	********************************************/
	int rV = 0;

	if (Accept(ExpectedToken, LookAHead, pIN))
		rV = Lex(pIN);
	else
	{
		char* pToken1 = new char[64],*pToken2 = new char[64];
		for (int i = 0; i < 64; ++i)
		{
			pToken1[i] = 0;
			pToken2[i] = 0;		//Expected Token String
		}
		if (LookAHead > 32 && LookAHead < 128)
			pToken1[0] = LookAHead;
		else if (LookAHead > 255)
		{
			sprintf_s(pToken1, 64, "%s", TokenLookup(LookAHead));
		}
		else
			sprintf_s(pToken1, 64, "%d", LookAHead);
		if (ExpectedToken > 32 && ExpectedToken < 128)
			pToken2[0] = ExpectedToken;
		else if (ExpectedToken > 255)
		{
			sprintf_s(pToken2, 64, "%s", TokenLookup(ExpectedToken));
		}
		else
			sprintf_s(pToken2, 64, "%d", ExpectedToken);
		sprintf_s(Exception.ErrorString, 256, "Syntax Error:Line %d Col %d  Expected %s Got %s\n",
			m_Line,
			m_Col,
			pToken2,
			pToken1
		);
		throw Exception;

	}
	return rV;
}

int CFileParser::Accept(int token, int LookAHead, FILE *pIN)
{
	int rV=0;

	if(token == LookAHead)
		rV = 1;
	return rV;
}

BOOL CFileParser::CheckKeywordChar(int c)
{
	BOOL rV = FALSE;

	if (isalnum(c) || c == '_')
		rV = TRUE;
	return rV;
}

int CFileParser::Lex(FILE *pIN)
{
	int loop,rV;
	int c;
	int token;
	loop = 1;
	while(loop)
	{
		c = GetChar(pIN);
		switch(c)
		{
			case EOF:
				loop = 0;
				rV = c;
				break;
			case ' ': case '\t':	//whitespace
				break;
			case '\n':	//end of line
				break;
			case '\"':	//string
				m_LexIndex = 0;
				while((c = GetChar(pIN)) != '\"')
				{
					m_LexBuff[m_LexIndex++] = c;
				}
				m_LexBuff[m_LexIndex] = 0;
				rV = TOKEN_STRING;
				loop = 0;
				break;
			case '0': case '1': case '2': case '3':
			case '4': case '5': case '6': case '7':
			case '8': case '9':	case '-': //deccimal number
				m_LexIndex = 0;
				do
				{
					m_LexBuff[m_LexIndex++] = c;
				}while(isdigit(c = GetChar(pIN)));
				UnGetChar(c);
				m_LexBuff[m_LexIndex] = 0;
				loop = 0;
				m_LexValue = atoi(m_LexBuff);
				rV = TOKEN_NUM;
				break;
			case ',': case '(': case ')': case '{': case '}':
			case '[':case ']':
				rV = c;
				loop = 0;
				break;
			default:	//keywords
				m_LexIndex = 0;
				m_LexBuff[m_LexIndex++] = c;
				while(CheckKeywordChar(c = GetChar(pIN)))
				{
					m_LexBuff[m_LexIndex++] = c;
				}
				UnGetChar(c);
				m_LexBuff[m_LexIndex] = 0;
				token = LookUp(m_LexBuff);
				if(token >= 0)
					rV = token;
				else
					rV = -1;
				loop = 0;
				break;
		}
	}
	return rV;
}

int CFileParser::Parse(FILE *pIN)
{
	// TODO:fix Drawing File Keyword parsing
	int loop = 1;
	int Token;
	CCadLibrary *pLib=0;

	try
	{
		Token = Lex(pIN);
		while (loop)
		{
			switch (Token)
			{
			case TOKEN_DRAWFILE:
				Token = DrawFile(pIN, Token, m_pDoc);
				break;
			case TOKEN_LIBFILE:
				pLib = new CCadLibrary;
				Token = Expect(TOKEN_LIBFILE, Token, pIN);
				Token = Expect('(', Token, pIN);
				pLib->SetName(m_LexBuff);
				Token = Expect(TOKEN_STRING, Token, pIN);
				Token = Expect(')', Token, pIN);
				Token = Expect('{', Token, pIN);
				Token = LibFile(pIN, Token, pLib);
				Token = Expect('}', Token, pIN);
				break;
			case EOF:
				loop = 0;
				if (pLib)
					this->m_pLib = pLib;
				break;
			}
		}
	}
	catch(SException& XCept)
	{
		MessageBox(NULL, XCept.ErrorString, "Error", MB_OK);
	}
	return 0;
}

int CFileParser::LibFile(FILE *pIN, int Token,CCadLibrary *pLib)
{
	CCadLibObject *pLibObj;
	int loop=1;
	while(loop)
	{
		switch(Token)
		{
			case TOKEN_LIBPART:
				pLibObj = new CCadLibObject;
//				Token = pLibObj->Parse(pIN, Token, , this);
				Token = LibPart(pIN,Token,pLibObj);
				pLib->AddPart(pLibObj);
				break;
			case '}':
				loop = 0;
				break;
		}
	}
	return Token;
}

int CFileParser::DrawFile(FILE *pIN,int Token, CFrontCadDoc*pDrawing)
{
	//------------------------------------------------------------------------
	//	DrawFile	-> DRAWFILE DrawFileAttributes '{' DrawObjects '}'
	//				-> .
	//------------------------------------------------------------------------
	int Loop = TRUE;
	CCadDrawing* pDraw = pDrawing->GetDrawing();
	Token = Expect(TOKEN_DRAWFILE, Token, pIN);
	pDraw->SetBkColor(RGB(255, 255, 255));	//default background
	Token = DrawFileAttributes(pIN, Token, pDraw);
	Token = Expect('{', Token, pIN);
	Token = DrawObjects(pIN, Token, pDraw);
	Token = Expect('}', Token, pIN);
	return Token;
}

int CFileParser::DrawFileAttributes(FILE* pIN, int Token, CCadObject* pO)
{
	//--------------------------------------------------------
	// DrawFileAttributes	-> '(' DrawFileAttributes1 ')'
	//						-> .
	//--------------------------------------------------------
	Token = Expect('(', Token, pIN);
	Token = DrawFileAttributes2(pIN, Token, pO);
	Token = Expect(')', Token, pIN);
	return Token;
}


int CFileParser::DrawFileAttributes2(FILE* pIN, int Token, CCadObject* pO)
{
	//--------------------------------------------------------------------
	//	DrawFileAttributes1	-> DrawingAttributes3 DrawingAttributes2;
	// 
	//	DrawingAttributes2	-> ',' DrawingAttributes3 DrawingAttributes2
	//						-> .
	//--------------------------------------------------------------------
	BOOL Loop = TRUE;

	Token = DrawFileAttributes3(pIN, Token, pO);
	while (Loop)
	{
		switch (Token)
		{
		case ',':
			Token = Expect(',', Token, pIN);
			Token = DrawFileAttributes3(pIN, Token, pO);
			break;
		default:
			Loop = FALSE;
			break;
		}
	}
	return Token;
}

int CFileParser::DrawFileAttributes3(FILE* pIN, int Token, CCadObject* pO)
{
	//-------------------------------------------------------------
	//	DrawFileAttributes3	-> COLOR '(' NUM ',' NUM ',' NUM ')'
	//						-> SCALE '(' NUM ')'
	//						-> .
	//-------------------------------------------------------------
	switch (Token)
	{
	case TOKEN_BACKGROUND_COLOR:
		Token = Color(Token, pIN, ((CCadDrawing*)pO)->m_BkColor, Token);
		break;
	case TOKEN_SCALE:
		Token = Expect(TOKEN_SCALE, Token, pIN);
		Token = Expect('(', Token, pIN);
		Token = Expect(TOKEN_NUM, Token, pIN);
		Token = Expect(')', Token, pIN);
		break;
	}
	return Token;
}

int CFileParser::PrintRect(FILE* pIN, int Token, CCadObject* pO)
{
	BOOL Loop = TRUE;
	CCadPrintRect* pPR;

	pPR = new CCadPrintRect;
	Token = Expect(TOKEN_PRINTRECT, Token, pIN);
	Token = Expect('(', Token, pIN);
	while (Loop)
	{
		switch (Token)
		{
		case TOKEN_POINT_1:
			Token = Point(TOKEN_POINT_1, pIN, pPR->GetP1(), Token);
			break;
		case TOKEN_COLOR:
			Token = Color(TOKEN_COLOR, pIN, pPR->m_Attrib.m_LineColor, Token);
			break;
		case TOKEN_LINE_WIDTH:
			Token = DecimalValue(TOKEN_LINE_WIDTH, pIN, pPR->m_Attrib.m_Width, Token);
			break;
		case TOKEN_SIZE:
			Token = Size(TOKEN_SIZE, pIN, pPR->m_Size, Token);
			break;
		case ',':
			Token = Expect(',', Token, pIN);
			break;
		case ')':
			Loop = FALSE;
			break;
		}//end of switch token
	}
	Token = Expect(')', Token, pIN);
	pO->AddObject(pPR);
	return Token;
}

int CFileParser::DrawObjects(FILE* pIN, int Token, CCadObject* pO)
{
	BOOL Loop = TRUE;
	ObjectTypes DrawingObjects;
	CCadDrawing* pDraw = (CCadDrawing*)pO;

	memset(&DrawingObjects, 0, sizeof(ObjectTypes));
	while (Loop)
	{
		switch (Token)
		{
		case TOKEN_PRINTRECT:
			DrawingObjects.pPrintRect = new CCadPrintRect;
			Token = DrawingObjects.pPrintRect->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_DIMENSION:
			DrawingObjects.pDim = new CCadDimension;
			Token = DrawingObjects.pDim->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_ORG:
			DrawingObjects.pOrigin = new CCadOrigin;
			Token = DrawingObjects.pOrigin->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_TEXT:
			DrawingObjects.pText = new CCadText;
			Token = DrawingObjects.pText->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_ARC:
			DrawingObjects.pArcObj = new CCadArc;
			Token = DrawingObjects.pArcObj->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_ARC_CENTER:
			DrawingObjects.pArcCentObj = new CCadArcCentered;
			Token = DrawingObjects.pArcCentObj->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_ELLIPSE:
			DrawingObjects.pElipse = new CCadElipse;
			Token = DrawingObjects.pElipse->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_CIRCLE:
			DrawingObjects.pCircle = new CCadCircle;
			Token = DrawingObjects.pCircle->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_RECT:
			DrawingObjects.pRect = new CCadRect;
			Token = DrawingObjects.pRect->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_RNDRECT:
			DrawingObjects.pRndedRect = new CCadRoundRect;
			Token = DrawingObjects.pRndedRect->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_POLY:
			DrawingObjects.pPoly = new CCadPolygon;
			Token = DrawingObjects.pPoly->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_POLYFILL:
			DrawingObjects.pPolyFill = new CCadPolygonFill;
			Token = DrawingObjects.pPolyFill->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_LINE:
			DrawingObjects.pLine = new CCadLine;
			Token = DrawingObjects.pLine->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_HOLERND:
			DrawingObjects.pRndHole = new CCadHoleRound;
			Token = DrawingObjects.pRndHole->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_HOLERECT:
			DrawingObjects.pRectHole = new CCadRectHole;
			Token = DrawingObjects.pRectHole->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_HOLE_1FLAT:
			DrawingObjects.pRnd1Flat = new CCadHoleRnd1Flat;
			Token = DrawingObjects.pRnd1Flat->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_HOLE_2FLAT:
			DrawingObjects.pRnd2Flat = new CCadHoleRnd2Flat;
			Token = DrawingObjects.pRnd2Flat->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_LIBPART:
			DrawingObjects.pLibObj = new CCadLibObject;
			Token = DrawingObjects.pLibObj->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_BITMAP:
			DrawingObjects.pBitMap = new CCadBitmap;
			Token = DrawingObjects.pBitMap->Parse(pIN, Token, &pDraw, this);
			break;
		case TOKEN_ARROW:
			DrawingObjects.pArrow = new CCadArrow;
			Token = DrawingObjects.pArrow->Parse(pIN, Token, &pDraw, this);
			break;
		case '}':
			Loop = 0;
			break;
		}//end of switch(Token)
	}	//end of while loop
	return Token;
}

//------------------------------------------
// Utility Parsing Methods
//------------------------------------------

int CFileParser::Point(int TargeToken, FILE* pIN, CPoint& Point, int Token)
{
	int x, y;

	Token = Expect(TargeToken, Token, pIN);
	Token = Expect('(', Token, pIN);
	x = atoi(m_LexBuff);
	Token = Expect(TOKEN_NUM, Token, pIN);
	Token = Expect(',', Token, pIN);
	y = atoi(m_LexBuff);
	Token = Expect(TOKEN_NUM, Token, pIN);
	Token = Expect(')', Token, pIN);
	Point = CPoint(x, y);
	fprintf(theApp.GetLog(), "CParser::Point(%d,%d)\n", x, y);
	return Token;
}

int CFileParser::LibPart(FILE *pIN, int Token,CCadLibObject *pLO)
{
	Token = Expect(TOKEN_LIBPART,Token,pIN);
	Token = Expect('(',Token,pIN);
	pLO->SetName(m_LexBuff);
	Token = Expect(TOKEN_STRING,Token,pIN);
	Token = Expect(',',Token,pIN);
	Token = Point(TOKEN_REFERENCE, pIN,pLO->GetRef(), Token);
	Token = Expect(',',Token,pIN);
	Token = Point(TOKEN_POINT_1, pIN,pLO->GetP1(), Token);
	Token = Expect(')',Token,pIN);
	Token = Expect('{',Token,pIN);
	Token = DrawObjects(pIN,Token,pLO);
	Token = Expect('}',Token,pIN);
	return Token;
}

int CFileParser::Color(int TargetToken, FILE *pIN, COLORREF& ColorParam,int Token)
{
	int RED,BLUE,GREEN;

	Token = Expect(TargetToken,Token,pIN);
	Token = Expect('(',Token,pIN);
	Token = Red(pIN, RED, Token);
	Token = Expect(',', Token, pIN);
	Token = Green(pIN, GREEN, Token);
	Token = Expect(',', Token, pIN);
	Token = Blue(pIN, BLUE, Token);
	Token = Expect(')',Token,pIN);
	ColorParam = RGB(RED,GREEN,BLUE);
	return Token;
}

int CFileParser::Red(FILE* pIN, int& red, int Token)
{
	Token = DecimalValue(TOKEN_RED, pIN, red, Token);
	return Token;
}

int CFileParser::Green(FILE* pIN, int& green, int Token)
{
	Token = DecimalValue(TOKEN_GREEN, pIN, green, Token);
	return Token;
}

int CFileParser::Blue(FILE* pIN, int& blue, int Token)
{
	Token = DecimalValue(TOKEN_BLUE, pIN, blue, Token);
	return Token;
}

int CFileParser::Font(FILE *pIN, char **F,int Token)
{
	Token = Expect(TOKEN_FONT,Token,pIN);
	if(Token >=0)Token = Expect('(',Token,pIN);
	if(Token >=0)
	{
		int len = strlen(m_LexBuff) + 1;
		*F = new char[len];
		strcpy_s(*F,len,m_LexBuff);
	}
	if(Token >=0)Token = Expect(TOKEN_STRING,Token,pIN);
	if(Token >=0)Token = Expect(')',Token,pIN);
	return Token;
}

int CFileParser::DecimalValue(int TargetToken, FILE* pIN, int &v, int Token)
{
	Token = Expect(TargetToken,Token,pIN);
	if(Token >=0)Token = Expect('(',Token,pIN);
	if(Token >=0)v = atoi(m_LexBuff);
	if(Token >=0)Token = Expect(TOKEN_NUM,Token,pIN);
	if(Token >=0)Token = Expect(')',Token,pIN);
	return Token;
}

int CFileParser::Size(int TargetToken, FILE * pIN, CSize & Sz, int Token)
{
	int x, y;

	Token = Expect(TargetToken, Token, pIN);
	if (Token >= 0)Token = Expect('(', Token, pIN);
	if (Token >= 0)x = atoi(m_LexBuff);
	if (Token >= 0)Token = Expect(TOKEN_NUM, Token, pIN);
	if (Token >= 0)Token = Expect(',', Token, pIN);
	if (Token >= 0)y = atoi(m_LexBuff);
	if (Token >= 0)Token = Expect(TOKEN_NUM, Token, pIN);
	if (Token >= 0)Token = Expect(')', Token, pIN);
	if (Token >= 0)Sz = CSize(x, y);
	return Token;
}

char* CFileParser::SaveString(char* s, int n, int Token, char* string)
{
    return s;
}

char* CFileParser::SaveSize(char* s, int n, int Token, CSize& size)
{
	sprintf_s(s, n, "%s(%d,%d)",
		TokenLookup(Token),
		size.cx,
		size.cy
	);
	return s;
}

char* CFileParser::SavePoint(char* s, int n, int Token, CPoint point)
{
	sprintf_s(s, n, "%s(%d,%d)",
		TokenLookup(Token),
		point.x, 
		point.y
	);
	return s;
}

char* CFileParser::SaveColor(char* s, int n, COLORREF color, int Token)
{
	sprintf_s(s, n, "%s(RED(%d),GREEN(%d),BLUE(%d))",
		TokenLookup(Token),
		RED(color),
		GREEN(color),
		BLUE(color)
	);
	return s;
}

char* CFileParser::SaveDecimalValue(char* s, int n, int Token, int& value)
{
	sprintf_s(s, n, "%s(%d)",
		TokenLookup(Token),
		value
	);
	return s;
}

char* CFileParser::SaveTransparent(char* s, int n, int Token, int& value)
{
	sprintf_s(s, n, "%s(%d)",
		TokenLookup(Token),
		value
	);
	return s;
}


