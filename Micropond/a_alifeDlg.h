// a_alifeDlg.h : header file
//

#include "afxwin.h"
#if !defined(AFX_A_ALIFEDLG_H__7C37756A_D99F_4A0F_820F_7CF3C7AA08D1__INCLUDED_)
#define AFX_A_ALIFEDLG_H__7C37756A_D99F_4A0F_820F_7CF3C7AA08D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAlifeDlg dialog

class CAlifeDlg : public CDialog
{
// Construction
public:
	void UpdateCounts ( void );
	CAlifeDlg(CWnd* pParent = NULL);	// standard constructor

	void DoReset1();
	void DoReset2();
	void DoReset3();
	void DoReset4();

	void TurnCrank();

// Dialog Data
	//{{AFX_DATA(CAlifeDlg)
	enum { IDD = IDD_A_ALIFE_DIALOG };
	CSliderCtrl	m_sliderDelay;
	CButton	m_ctrlShowDisplay;
	CSliderCtrl	m_sliderDivideCost;
	CStatic	m_ctrlNames;
	CStatic	m_ctrlCounts;
	CSliderCtrl	m_sliderFoodProduction;
	CSliderCtrl	m_sliderFoodValue;
	CSliderCtrl	m_sliderMutationRate;
	CButton	m_ctrlDisplayArea;
	BOOL	m_bShowDisplay;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlifeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
public:
	HICON m_hIcon;
	CRect m_rectDisplay;

	// Generated message map functions
	//{{AFX_MSG(CAlifeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnReset();
	virtual void OnOK();
	afx_msg void OnResetBarriers1();
	afx_msg void OnResetBarriers2();
	afx_msg void OnResetBarriers3();
	afx_msg void OnResetBarriers4();
	afx_msg void OnInsert();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSave();
	afx_msg void OnLoad();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnOutofmemorySliderTurnCost(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void	GetControlValues ( void );
	afx_msg void OnNMCustomdrawSliderFoodProduction(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedInstructionset();
	CStatic m_editTimeMS;
	CButton m_checkMagnifyCenter;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_A_ALIFEDLG_H__7C37756A_D99F_4A0F_820F_7CF3C7AA08D1__INCLUDED_)
