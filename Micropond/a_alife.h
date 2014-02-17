// a_alife.h : main header file for the A_ALIFE application
//

#if !defined(AFX_A_ALIFE_H__74390CF2_C246_4F2A_8D27_90A8B1626E45__INCLUDED_)
#define AFX_A_ALIFE_H__74390CF2_C246_4F2A_8D27_90A8B1626E45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CAlifeApp:
// See a_alife.cpp for the implementation of this class
//

class CAlifeApp : public CWinApp
{
public:
	CAlifeApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlifeApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAlifeApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_A_ALIFE_H__74390CF2_C246_4F2A_8D27_90A8B1626E45__INCLUDED_)
