#if !defined(AFX_INSERTDLG_H__58DE8C75_DBD9_48BB_94D3_2366FC76B71B__INCLUDED_)
#define AFX_INSERTDLG_H__58DE8C75_DBD9_48BB_94D3_2366FC76B71B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InsertDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInsertDlg dialog

class CInsertDlg : public CDialog
{
// Construction
public:
	CInsertDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInsertDlg)
	enum { IDD = IDD_INSERT_ORGANISM_DLG };
	CString	m_strCode;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInsertDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInsertDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSERTDLG_H__58DE8C75_DBD9_48BB_94D3_2366FC76B71B__INCLUDED_)
