/*
    This file is part of Micropond v1.0 (aka The Evolving Sandbox).
	author: Scott Schafer, scott.schafer@gmail.com

    Micropond v1.0 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Foobar is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Micropond v1.0.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "a_alife.h"
#include "InsertDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInsertDlg dialog


CInsertDlg::CInsertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInsertDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInsertDlg)
	m_strCode = _T("");
	//}}AFX_DATA_INIT
}


void CInsertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertDlg)
	DDX_Text(pDX, IDC_EDIT1, m_strCode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInsertDlg, CDialog)
	//{{AFX_MSG_MAP(CInsertDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsertDlg message handlers
