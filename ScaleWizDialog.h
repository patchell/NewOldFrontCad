#if !defined(AFX_SCALEWIZDIALOG_H__79AD2FCC_7248_497C_B1C0_BB55E57356D6__INCLUDED_)
#define AFX_SCALEWIZDIALOG_H__79AD2FCC_7248_497C_B1C0_BB55E57356D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScaleWizDialog.h : header file
//
#define SCALE_LABELS_SIZE		40
#define SCALE_LABELS_STRING_SIZE	32

#include "resource.h"
#include "StaticPreview.h"
#include "ComboBoxHoleType.h"
#include "ComboBoxLibrary.h"

struct SCALEWIZattributes {
	int m_ArcStart;
	int m_ArcEnd;
	int m_ArcLineWidth;
	int m_ArcRadius;
	COLORREF m_ArcColor;
	int m_Divisions;
	int m_DivisionLineWidth;
	int m_DivisionLineLegnth;
	COLORREF m_DivLineColor;
	int m_SubDivisions;
	int m_SubDivisionLineWidth;
	int m_SubdivisionLengnth;
	COLORREF m_SubDivColor;
	int m_HoleSize;
	int m_HoleType;
	int m_FlatDist;
	CPoint m_Ref;		///reference point for scale object
	///scale labels
	int m_FontSize;
	int m_FontWeight;
	int m_DistToTick;
	COLORREF m_TextColor;
	COLORREF m_BkColor;
	char *m_pFont;		//name of font.
	char *m_pLabels[SCALE_LABELS_SIZE];
	SCALEWIZattributes() {
		m_ArcStart=135;
		m_ArcEnd=45;
		m_ArcLineWidth=2;
		m_ArcRadius=100;
		m_ArcColor=RGB(0,0,0);
		m_Divisions=10;
		m_DivisionLineWidth=2;
		m_DivisionLineLegnth=10;
		m_DivLineColor=RGB(0,0,0);
		m_SubDivisions=5;
		m_SubDivisionLineWidth=1;
		m_SubdivisionLengnth=5;
		m_SubDivColor=RGB(0,0,0);
		m_HoleSize=5;
		m_HoleType=0;
		m_FlatDist=7;
		m_Ref=CPoint(0,0);
		m_FontSize=12;
		m_FontWeight=400;
		m_DistToTick=15;
		m_TextColor=RGB(0,0,0);
		m_BkColor=RGB(255,255,255);
		m_pFont=_strdup("Arial");
		for (int i=0; i<SCALE_LABELS_SIZE; i++) {
			sprintf(m_pLabels[i],"%.2f",i*1.0);
		}
	}
};

/////////////////////////////////////////////////////////////////////////////
// CScaleWizDialog dialog

class CScaleWizDialog : public CDialog
{
	double m_Pi;
	CEdit	m_Edit_YRef;
	CEdit	m_Edit_XRef;
	CComboBoxLibrary	m_Combo_SelLib;
	CEdit	m_Edit_PartName;
	CMyTabCtrl	m_Tab_Settings;
	CStaticPreview	m_Static_Preview;
	int m_RotateTextFlag;
	int m_PrevLabel;
public:
	enum { IDD = IDD_DIALOG_SCALEWIZ };
	CScaleWizDialog(CWnd* pParent = NULL);   // standard constructor
	virtual  ~CScaleWizDialog();
	CPoint CalcTextShiftonRotation(CPoint p1, CPoint Center, double angle);
	CPoint CalcXYonArc(int Radius, double Angle);
	CPoint CalcXYonArc(int Radius, int Angle);
	CCadLibObject *CreateLibObject(const char *name);
	//}}AFX_DATA

	SCALEWIZattributes m_atrb;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	LRESULT OnLabelSelScrollBar(WPARAM Item,LPARAM lp);
	LRESULT OnTab1Message(WPARAM wP, LPARAM lP);
	LRESULT OnTab2Message(WPARAM wP, LPARAM lP);
	LRESULT OnTab3Message(WPARAM wP, LPARAM lP);

	// Generated message map functions
	//{{AFX_MSG(CScaleWizDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSelchangeTabScalewiz(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonWizSavetolib();
	afx_msg void OnButtonScalewizNewlib();
	afx_msg void OnSelchangeComboScalewizLibsel();
	afx_msg void OnButtonScalewizSetref();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif // !defined(AFX_SCALEWIZDIALOG_H__79AD2FCC_7248_497C_B1C0_BB55E57356D6__INCLUDED_)
