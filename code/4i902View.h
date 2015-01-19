// 4i902View.h : interface of the Ci902View class
//


#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Chart.h"
typedef unsigned long double BIG;

class Ci902View : public CFormView
{
protected: // create from serialization only
	Ci902View();
	DECLARE_DYNCREATE(Ci902View)

public:
	enum{ IDD = IDD_I902_FORM };

// Attributes
public:
	Ci902Doc* GetDocument() const;

// Operations
public:

// Overrides
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~Ci902View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	void DrawCliquesUnEqual();
	void UnEqualSizeClique();
	void SetProgressBar();
	void GetUserDensityClique();
	void HighlightClique();
	int m_NoOfVeriticesL;
	int m_NoOfVeriticesM;
	int m_DensityOfGraph;
	int **graphMatrix;
	int maxEdgesAllowed;
	int noOfEdgesPresent;
	int redrawCounter;
	int clearPressed;
	unsigned long double globalSum,re;
	afx_msg void OnDisplayGraph();
	//void DrawGraph();
	
	void DrawGraph(void);
	afx_msg void OnPaint();
	void PrepareForCombination(void);
	unsigned long double GetFactorial(int num);
	unsigned long double Factorial(int num);
	int GetCombination(int *n_Start, int *n_End,int *r_Start, int *r_End);

	CListBox m_ListM;
	CListBox m_ListL;
	int topL[5000],uTopL[5000];
	int bottomM[5000],uBottomM[5000];
	int indexL,indexM,uIndexL,uIndexM;
	afx_msg void OnBnClickedButton2();
	void DrawCliques(void);
	float m_CliqueDensity;
	afx_msg void OnBnClickedButtondensitytimegraph();
	afx_msg void OnBnClickedButton4();
	CProgressCtrl m_ProgressBar;
	CChart m_Chart2d;
	unsigned long totalTime;
	int gDensity[20];
	unsigned long gTime[20];
	int gIndex;
	int progressing;
	int unEqualGraph;
	afx_msg void OnBnClickedUnequalclique();
	afx_msg void OnBnClickedTogglebutton();
	afx_msg void OnBnClickedButton6();
};

#ifndef _DEBUG  // debug version in 4i902View.cpp
inline Ci902Doc* Ci902View::GetDocument() const
   { return reinterpret_cast<Ci902Doc*>(m_pDocument); }
#endif

