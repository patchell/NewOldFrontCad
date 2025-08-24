// FileParser.h: interface for the CFileParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEPARSER_H__6A884CBF_F824_4989_ABE1_0126A175C1F8__INCLUDED_)
#define AFX_FILEPARSER_H__6A884CBF_F824_4989_ABE1_0126A175C1F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum {
	TOKEN_NUM=256,
	TOKEN_STRING,			//1
	TOKEN_ARC,				//2
	TOKEN_ARC_CENTER,		//3
	TOKEN_CIRCLE,			//4
	TOKEN_ELLIPSE,
	TOKEN_RECT,				//5
	TOKEN_RNDRECT,			//6
	TOKEN_POLY,				//7
	TOKEN_POLYFILL,			//8
	TOKEN_LINE,				//9
	TOKEN_HOLERND,			//10
	TOKEN_HOLERECT,			//11
	TOKEN_HOLE_1FLAT,		//12
	TOKEN_HOLE_2FLAT,		//13
	TOKEN_LIBPART,			//14
	TOKEN_TEXT,				//15
//	TOKEN_POINT,			//16
	TOKEN_POINT_1,
	TOKEN_POINT_2,
	TOKEN_POINT_3,
	TOKEN_VERTEX_POINT,
	TOKEN_START_POINT,
	TOKEN_END_POINT,
	//colors
	TOKEN_BACKGROUND_COLOR,	//17
	TOKEN_LINE_COLOR,
	TOKEN_TEXT_COLOR,
	TOKEN_FILL_COLOR,
	TOKEN_RED,
	TOKEN_GREEN,
	TOKEN_BLUE,
	TOKEN_COLOR,
	//-------
	TOKEN_FONT,
	TOKEN_WEIGHT,
	TOKEN_FONTHIEGHT,
	TOKEN_FONTWIDTH,
	TOKEN_RADIUS,
	TOKEN_FLATDIST,
	TOKEN_DRAWFILE,
	TOKEN_LIBFILE,
	TOKEN_VERTEX,
	TOKEN_RECTWIDTH,
	TOKEN_RECTHIEGHT,
	TOKEN_ANGLE,
	TOKEN_TRANSPARENT,
	TOKEN_REFERENCE,
	TOKEN_BITMAP,
	TOKEN_FILE,
	TOKEN_ARROW,
	TOKEN_LENGTH,
	TOKEN_ORG,	//token for origin object
	TOKEN_DIMENSION,
	TOKEN_PRINTRECT,
	TOKEN_SIZE,
	TOKEN_SCALE,
	TOKEN_LINE_WIDTH,
	TOKEN_ARROW_LENGTH,
	TOKEN_ARROW_WIDTH
};

typedef struct {
	const char *KeyWord;
	int Token;
}KEYWORD;

class CFileParser  
{
	CFrontCadDoc *m_pDoc;
	CCadLibrary *m_pLib;
	CCadLibObject *m_pLob;
	CCadLibObject *m_pDLob;
	int m_LexIndex;
	char m_LexBuff[256];
	int m_LexValue;
	int UnGetBuff;
	int m_Col;
	int m_Line;
public:
	CFileParser(CFrontCadDoc *pD);
	CFileParser();
	virtual ~CFileParser();
	//-------------------------------------
	// Parsing Utillity Methods
	//-------------------------------------
	int LibPart(FILE *pIN, int Token,CCadLibObject *pLibobj);
	int Color(int TargetToken, FILE* pIN, COLORREF& C, int Token);
	int Red(FILE* pIN, int& Color, int token);
	int Green(FILE* pIN, int& Color, int token);
	int Blue(FILE* pIN, int& Color, int token);
	int DecimalValue(int TargetToken, FILE* pIN, int& v, int Token);
	int Font(FILE *pIN, char **F,int token);
	int Point(int TargeToken, FILE* pIN, CPoint& Point, int Token);
	int Size(int TargetToken, FILE* pIN, CSize& Sz, int Token);
	//----------------------------------------------------
	// Parsing Methods
	//----------------------------------------------------
	int LibFile(FILE *pIN, int Token,CCadLibrary *pLib);
	int DrawFile(FILE *pIN,int Token, CFrontCadDoc*pDrawing);
	int DrawFileAttributes(FILE* pIN, int Token, CCadObject* pO);
	int DrawFileAttributes2(FILE* pIN, int Token, CCadObject* pO);
	int DrawFileAttributes3(FILE* pIN, int Token, CCadObject* pO);
	int PrintRect(FILE* pIN, int Token, CCadObject* pO);
	int DrawObjects(FILE* pIN, int Token, CCadObject* pO);
	//--------------------------------------------
	// Lexical Methods
	//--------------------------------------------
	int Expect(int token, int lookahead, FILE *pIN);
	int Accept(int token, int lookahead, FILE *pIN);
	int LookUp(char *pKW);
	static const char* TokenLookup(int Token);
	int UnGetChar(int c);
	int GetChar(FILE* in);
	BOOL CheckKeywordChar(int c);
	int Lex(FILE* pIN);
	char* GetLexBuff() { return m_LexBuff; }
	int GetLexValue() { return m_LexValue; }
	int GetLine() { return m_Line; }
	int GetCol() { return m_Col; }
	//-------------------------------------------
	int Open(char *pFileName,CFrontCadDoc *pDoc);
	int Parse(FILE *pIN);
	CCadLibrary *GetLib(void){return m_pLib;}
	//----------------------------------------
	// File Save Utils
	//----------------------------------------
	static char* SaveString(char* s, int n, int Token, char* string);
	static char* SaveSize(char* s, int n, int Token, CSize& size);
	static char* SavePoint(char* s, int n, int Token, CPoint point);
	static char* SaveColor(char* s, int n, COLORREF color, int Token);
	static char* SaveDecimalValue(char* s, int n, int Token, int& value);
	static char* SaveTransparent(char* s, int n, int Token, int& value);
};


#endif // !defined(AFX_FILEPARSER_H__6A884CBF_F824_4989_ABE1_0126A175C1F8__INCLUDED_)
