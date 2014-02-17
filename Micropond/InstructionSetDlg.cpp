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
#include "InstructionSetDlg.h"
#include "globals.h"

// CInstructionSetDlg dialog

IMPLEMENT_DYNAMIC(CInstructionSetDlg, CDialog)

CInstructionSetDlg::CInstructionSetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInstructionSetDlg::IDD, pParent)
	, m_bInstructionM(false)
	, m_bInstructionZ(false)
	, m_bInstructionA(false)
	, m_bInstructionRotLeft(false)
	, m_bInstructionRotRight(false)
	, m_bInstructionU(false)
	, m_bInstructionD(false)
	, m_bInstructionR(false)
	, m_bInstructionL(false)
	, m_bInstruction0(false)
	, m_bInstruction1(false)
	, m_bInstruction2(false)
	, m_bInstruction3(false)
	, m_bInstruction4(false)
	, m_bInstruction5(false)
	, m_bInstruction6(false)
	, m_bInstruction7(false)
	, m_bInstruction8(false)
	, m_bInstruction9(false)
	, m_bInstructionE(false)
	, m_bInstructione(false)
	, m_bInstructionW(false)
	, m_bInstructionH(false)
	, m_bInstructionMarkHead(false)
	, m_bInstructionRestart(false)
{

}

CInstructionSetDlg::~CInstructionSetDlg()
{
}

void CInstructionSetDlg::DoDataExchange(CDataExchange* pDX)
{
	if (  ! pDX->m_bSaveAndValidate )
	{
		m_bInstructionM = ( gStrAllowedInstructions.Find ( "M" ) != -1 );
		m_bInstructionZ = ( gStrAllowedInstructions.Find ( "Z" ) != -1 );
		m_bInstructionA = ( gStrAllowedInstructions.Find ( "A" ) != -1 );
		m_bInstructionRotLeft = ( gStrAllowedInstructions.Find ( "<" ) != -1 );
		m_bInstructionRotRight = ( gStrAllowedInstructions.Find ( ">" ) != -1 );
		m_bInstructionU = ( gStrAllowedInstructions.Find ( "U" ) != -1 );
		m_bInstructionD = ( gStrAllowedInstructions.Find ( "D" ) != -1 );
		m_bInstructionR = ( gStrAllowedInstructions.Find ( "R" ) != -1 );
		m_bInstructionL = ( gStrAllowedInstructions.Find ( "L" ) != -1 );
		m_bInstruction0 = ( gStrAllowedInstructions.Find ( "0" ) != -1 );
		m_bInstruction1 = ( gStrAllowedInstructions.Find ( "1" ) != -1 );
		m_bInstruction2 = ( gStrAllowedInstructions.Find ( "2" ) != -1 );
		m_bInstruction3 = ( gStrAllowedInstructions.Find ( "3" ) != -1 );
		m_bInstruction4 = ( gStrAllowedInstructions.Find ( "4" ) != -1 );
		m_bInstruction5 = ( gStrAllowedInstructions.Find ( "5" ) != -1 );
		m_bInstruction6 = ( gStrAllowedInstructions.Find ( "6" ) != -1 );
		m_bInstruction7 = ( gStrAllowedInstructions.Find ( "7" ) != -1 );
		m_bInstruction8 = ( gStrAllowedInstructions.Find ( "8" ) != -1 );
		m_bInstruction9 = ( gStrAllowedInstructions.Find ( "9" ) != -1 );
		m_bInstructionE = ( gStrAllowedInstructions.Find ( "E" ) != -1 );
		m_bInstructione = ( gStrAllowedInstructions.Find ( "e" ) != -1 );
		m_bInstructionW = ( gStrAllowedInstructions.Find ( "W" ) != -1 );
		m_bInstructionH = ( gStrAllowedInstructions.Find ( "H" ) != -1 );
		m_bInstructionMarkHead = ( gStrAllowedInstructions.Find ( "*" ) != -1 );
		m_bInstructionRestart = ( gStrAllowedInstructions.Find ( "r" ) != -1 );
	}
	//{{AFX_DATA_MAP(CAlifeDlg)
	DDX_Check(pDX, IDC_CHECKM, m_bInstructionM );
	DDX_Check(pDX, IDC_CHECKZ, m_bInstructionZ );

	DDX_Check(pDX, IDC_CHECKA, m_bInstructionA );
	DDX_Check(pDX, IDC_CHECKROTLEFT, m_bInstructionRotLeft );
	DDX_Check(pDX, IDC_CHECKROTRIGHT, m_bInstructionRotRight );

	DDX_Check(pDX, IDC_CHECKU, m_bInstructionU );
	DDX_Check(pDX, IDC_CHECKD, m_bInstructionD );
	DDX_Check(pDX, IDC_CHECKR, m_bInstructionR );
	DDX_Check(pDX, IDC_CHECKL, m_bInstructionL );

	DDX_Check(pDX, IDC_CHECK0, m_bInstruction0 );
	DDX_Check(pDX, IDC_CHECK1, m_bInstruction1 );
	DDX_Check(pDX, IDC_CHECK2, m_bInstruction2 );
	DDX_Check(pDX, IDC_CHECK3, m_bInstruction3 );
	DDX_Check(pDX, IDC_CHECK4, m_bInstruction4 );
	DDX_Check(pDX, IDC_CHECK5, m_bInstruction5 );
	DDX_Check(pDX, IDC_CHECK6, m_bInstruction6 );
	DDX_Check(pDX, IDC_CHECK7, m_bInstruction7 );
	DDX_Check(pDX, IDC_CHECK8, m_bInstruction8 );
	DDX_Check(pDX, IDC_CHECK9, m_bInstruction9 );

	DDX_Check(pDX, IDC_CHECKE, m_bInstructionE );
	DDX_Check(pDX, IDC_CHECKee, m_bInstructione );
	DDX_Check(pDX, IDC_CHECKW, m_bInstructionW );
	DDX_Check(pDX, IDC_CHECKH, m_bInstructionH );
	DDX_Check(pDX, IDC_CHECKMARKHEAD, m_bInstructionMarkHead );
	DDX_Check(pDX, IDC_CHECKrr, m_bInstructionRestart );
	//}}AFX_DATA_MAP
	CDialog::DoDataExchange(pDX);

	if (  pDX->m_bSaveAndValidate )
	{
		CString strInstructions;
		if ( m_bInstructionM ) strInstructions += "M";
		if ( m_bInstructionZ ) strInstructions += "Z";
		if ( m_bInstructionA ) strInstructions += "A";
		if ( m_bInstructionRotLeft ) strInstructions += "<";
		if ( m_bInstructionRotRight ) strInstructions += ">";

		if ( m_bInstructionU ) strInstructions += "U";
		if ( m_bInstructionD ) strInstructions += "D";
		if ( m_bInstructionR ) strInstructions += "R";
		if ( m_bInstructionL ) strInstructions += "L";

		if ( m_bInstruction0 ) strInstructions += "0";
		if ( m_bInstruction1 ) strInstructions += "1";
		if ( m_bInstruction2 ) strInstructions += "2";
		if ( m_bInstruction3 ) strInstructions += "3";
		if ( m_bInstruction4 ) strInstructions += "4";
		if ( m_bInstruction5 ) strInstructions += "5";
		if ( m_bInstruction6 ) strInstructions += "6";
		if ( m_bInstruction7 ) strInstructions += "7";
		if ( m_bInstruction8 ) strInstructions += "8";
		if ( m_bInstruction9 ) strInstructions += "9";

		if ( m_bInstructionE ) strInstructions += "E";
		if ( m_bInstructione ) strInstructions += "e";
		if ( m_bInstructionW ) strInstructions += "W";
		if ( m_bInstructionH ) strInstructions += "H";
		if ( m_bInstructionMarkHead ) strInstructions += "*";
		if ( m_bInstructionRestart ) strInstructions += "r";

		gStrAllowedInstructions = strInstructions;
	}
}
