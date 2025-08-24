// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__E9CF1AC9_DCF9_4F75_A090_0C41D7516327__INCLUDED_)
#define AFX_STDAFX_H__E9CF1AC9_DCF9_4F75_A090_0C41D7516327__INCLUDED_

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include "afxdialogex.h"

#include "resource.h"       // main symbols

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include "Scale.h"
#include "Exception.h"
#include "FrontCadDefines.h"
//draw objects
#include "CadObject.h"
#include "CadArc.h"
#include "CadArcCentered.h"
#include "CadArc.h"
#include "CadBitmap.h"
#include "CadCircle.h"
#include "CadElipse.h"
#include "CadHoleRnd1Flat.h"
#include "CadHoleRnd2Flat.h"
#include "CadHoleRound.h"
#include "CadLibObject.h"
#include "CadOrigin.h"
#include "CadPolygon.h"
#include "CadPolygonFill.h"
#include "CadArrow.h"
#include "CadLine.h"
#include "CadPrintRect.h"
#include "CadRect.h"
#include "CadRectHole.h"
#include "CadRoundRect.h"
#include "CadText.h"
#include "CadDimension.h"
#include "CadDrawing.h"
#include "CadLibrary.h"


//controls
#include "ButtonMsg.h"
#include "StaticBitmap.h"
#include "StaticPreview.h"
#include "MyEdit.h"
#include "EditDecimal.h"
#include "ComboBoxPrintRect.h"
#include "ComboBoxHoleType.h"
#include "ComboBoxLibPartSelector.h"
#include "ComboBoxFontWeight.h"
#include "MyComboBox.h"
#include "HorzScrollBar.h"
#include "ComboBoxLibrary.h"
#include "MyBitmap.h"

#include "MoveObjects.h"

//mfc stuff
#include "DialogChoose.h"
#include "DialogGridSettings.h"
#include "DialogRenderEnable.h"
#include "NewLibDialog.h"

#include "MyTabCtrl.h"
#include "ScaleWizTabOne.h"
#include "WizTab2Dialog.h"
#include "WizTab3Dialog.h"
#include "ScaleWizDialog.h"
#include "SaveToLibDlg.h"

#include "RepeatDlg.h"
#include "SaveToLibDlg.h"

#include "SplitterUtility.h"
#include "LibSplitterWnd.h"
#include "LibFormView.h"
#include "UtilView.h"
#include "ruler.h"
#include "MainFrm.h"
#include "FrontCadDoc.h"
#include "FrontCadView.h"
#include "FrontCad.h"

#include "FileParser.h"

union ObjectTypes {
	CCadObject* pCadObj;
	CCadArc* pArcObj;
	CCadArcCentered* pArcCentObj;
	CCadArrow* pArrow;
	CCadBitmap* pBitMap;
	CCadDimension* pDim;
	CCadDrawing* pDrawing;
	CCadElipse* pElipse;
	CCadCircle* pCircle;
	CCadHoleRnd1Flat* pRnd1Flat;
	CCadHoleRnd2Flat* pRnd2Flat;
	CCadHoleRound* pRndHole;
	CCadLibObject* pLibObj;
	CCadLibrary* pLibrary;
	CCadLine* pLine;
	CCadOrigin* pOrigin;
	CCadPolygon* pPoly;
	CCadPolygonFill* pPolyFill;
	CCadPrintRect* pPrintRect;
	CCadRect* pRect;
	CCadRectHole* pRectHole;
	CCadRoundRect* pRndedRect;
	CCadText* pText;
};
#endif // !defined(AFX_STDAFX_H__E9CF1AC9_DCF9_4F75_A090_0C41D7516327__INCLUDED_)
