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
#include <afxmt.h>
#include "a_alife.h"
#include "a_alifeDlg.h"
#include "Board.h"
#include "OffscreenBuffer.h"
#include "constants.h"
#include "Reporter.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


FILE * pLogFile = NULL;

COffscreenBuffer gOffscreenBuffer;
bool gSuspendBoard = false;
bool gInTimer = false;
int gResetHit = 0;
int gSpeed = 0;

class CInTimerBlock
{
public:
	CInTimerBlock() { gInTimer = true; }
	~CInTimerBlock() { gInTimer = false; }
};

#define USE_MUTEX 0
#if ! USE_MUTEX
class CSuspendBoardBlock
{
public:
	CSuspendBoardBlock() 
	{
		gSuspendBoard = true; 
		extern CAlifeApp theApp;

		int count = 250;
		while ( gInTimer )
		{
			theApp.PumpMessage();
			Sleep(1);
			if ( --count == 0 )
				break;
		}
		ASSERT ( ! gInTimer );
	}
	~CSuspendBoardBlock() { gSuspendBoard = false; }
};
#else
CCriticalSection gThreadLock;
class CSuspendBoardBlock : public CSingleLock
{
public:
	CSuspendBoardBlock() : CSingleLock ( &gThreadLock, TRUE ) {}// { m_singleLock.Lock(); }

};
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlifeDlg dialog

CAlifeDlg::CAlifeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlifeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAlifeDlg)
	m_bShowDisplay = TRUE;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAlifeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlifeDlg)
	DDX_Control(pDX, IDC_SLIDER_DELAY, m_sliderDelay);
	//	DDX_Control(pDX, IDC_SHOW_DISPLAY, m_ctrlShowDisplay);
	//	DDX_Control(pDX, IDC_SLIDER_DIVIDE_COST, m_sliderDivideCost);
	DDX_Control(pDX, IDC_NAMES, m_ctrlNames);
	DDX_Control(pDX, IDC_COUNTS, m_ctrlCounts);
	DDX_Control(pDX, IDC_SLIDER_FOOD_PRODUCTION, m_sliderFoodProduction);
	DDX_Control(pDX, IDC_SLIDER_FOOD_VALUE, m_sliderFoodValue);
	DDX_Control(pDX, IDC_SLIDER_MUTATION_RATE, m_sliderMutationRate);
	DDX_Control(pDX, IDC_DISPLAY, m_ctrlDisplayArea);
	//	DDX_Check(pDX, IDC_SHOW_DISPLAY, m_bShowDisplay);
	//}}AFX_DATA_MAP

	m_ctrlDisplayArea.GetWindowRect ( &m_rectDisplay );

	m_sliderFoodProduction.SetRange ( 0, 100 );
	m_sliderFoodProduction.SetPos ( gFoodProduction );

	m_sliderFoodValue.SetRange ( 0, 100 );
	m_sliderFoodValue.SetPos ( gFoodValue );

	m_sliderMutationRate.SetRange ( 0, 100 );
	m_sliderMutationRate.SetPos ( gMutation );

	m_sliderDelay.SetRange ( 0, 100 );
	m_sliderDelay.SetPos ( 100 );

	/*
	m_sliderMovePenalty.SetRange ( 0, 100 );
	m_sliderMovePenalty.SetPos ( gMovePenalty );

	m_sliderMortality.SetRange ( 0, 100 );
	m_sliderMortality.SetPos ( gMortality );

	m_sliderTurnCost.SetRange ( 0, 100 );
	m_sliderTurnCost.SetPos ( gTurnCost );

	m_sliderDivideCost.SetRange ( 0, 20 );
	m_sliderDivideCost.SetPos ( gDivideEnergy );
	*/
	DDX_Control(pDX, IDC_TIME, m_editTimeMS);
	DDX_Control(pDX, IDC_CHECK1, m_checkMagnifyCenter);
};

BEGIN_MESSAGE_MAP(CAlifeDlg, CDialog)
	//{{AFX_MSG_MAP(CAlifeDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_BN_CLICKED(IDC_RESET_BARRIERS1, OnResetBarriers1)
	ON_BN_CLICKED(IDC_RESET_BARRIERS2, OnResetBarriers2)
	ON_BN_CLICKED(IDC_RESET_BARRIERS4, OnResetBarriers4)
	ON_BN_CLICKED(IDC_RESET_BARRIERS3, OnResetBarriers3)
	ON_BN_CLICKED(IDINSERT, OnInsert)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_FOOD_PRODUCTION, &CAlifeDlg::OnNMCustomdrawSliderFoodProduction)
	ON_BN_CLICKED(IDINSTRUCTIONSET, &CAlifeDlg::OnBnClickedInstructionset)
END_MESSAGE_MAP()

bool gRunThread = true;

UINT RunBoardThread ( LPVOID Param ) //Sample function for using in AfxBeginThread
{
	CAlifeDlg * pDlg = (CAlifeDlg*) Param;
	while ( gRunThread )
	{
		if ( !gSuspendBoard )
		{
			pDlg->TurnCrank();
		}
		else
			::Sleep(10);
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CAlifeDlg message handlers

BOOL CAlifeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	gOffscreenBuffer.Allocate ( 256, 256, 2 );
	gBoard.SetPixelBuffer ( (WORD*) gOffscreenBuffer.GetBaseAddress() );
	gBoard.Reset(0);
	//SetTimer( 1, 1, NULL );

//	pLogFile = fopen ( "log.txt", "w" );

    AfxBeginThread(RunBoardThread,this,THREAD_PRIORITY_NORMAL,0,0,NULL);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAlifeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
//		CAboutDlg dlgAbout;
//		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAlifeDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

#ifndef _DEBUG
	static bool bAboutShown = false;

	if ( ! bAboutShown )
	{
		bAboutShown = true;
		CSuspendBoardBlock sbb;
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
#endif
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAlifeDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAlifeDlg::TurnCrank() 
{
#if USE_MUTEX
	CSuspendBoardBlock ssb;
#endif

	CInTimerBlock itb;

	if ( gSuspendBoard )
		return;

	if ( gSpeed != 100 )
		::Sleep ( 100 - gSpeed );

	static bool bRecurse = false;
	if ( bRecurse )
		return;
	bRecurse = true;

	if ( gResetHit )
	{
		int reset = gResetHit;
		gResetHit = 0;
		switch ( reset )
		{
		case 1:
			DoReset1();
			break;

		case 2:
			DoReset2();
			break;

		case 3:
			DoReset3();
			break;

		case 4:
			DoReset4();
			break;
		}
	}

#if 1
	DWORD time = ::GetTickCount();
	static DWORD lastGetControlValuesTime = 0;
	if ( ( time - lastGetControlValuesTime ) > 500 )
	{
		GetControlValues();
		lastGetControlValuesTime = time;
	}
#endif

/*
	gFoodProduction =	m_sliderFoodProduction.GetPos();
	gFoodValue =		m_sliderFoodValue.GetPos();
	gMutation =			m_sliderMutationRate.GetPos();
	m_bShowDisplay =	m_ctrlShowDisplay.GetCheck();
*/

	bool bTakeTurn = true;

	if ( bTakeTurn )
	{
		DWORD beforeTurnTimeMS = ::GetTickCount();	
		gBoard.TakeTurn();
		DWORD elapsedTurnTimeMS = ::GetTickCount() - beforeTurnTimeMS;
#if 0
		static DWORD lastelapsedTurnTimeMS = DWORD(-1);
		if ( lastelapsedTurnTimeMS != elapsedTurnTimeMS )
		{
			lastelapsedTurnTimeMS = elapsedTurnTimeMS;
			CString strTimeMsg;
			strTimeMsg.Format ( "Turn took: %d MS", elapsedTurnTimeMS );
			m_editTimeMS.SetWindowText ( strTimeMsg );
		}
#endif
		bool bUpdateCounts = false;

		static DWORD lastUpdateCountTime = 0;
		if ( ( time - lastUpdateCountTime ) > 250 )
		{
			bUpdateCounts = true;
			lastUpdateCountTime = time;
			UpdateCounts();
		}

		static DWORD lastDrawTime = 0;
		bool bDraw = false;
		if ( ( time - lastDrawTime ) > ( 1000 / 50 ) )
		{
			bDraw = true;
			lastDrawTime = time;
		}

		if ( bDraw )
		{
			CRect r ( m_rectDisplay );
			r.DeflateRect ( 4, 12 );

			r.right = r.left + 256;
			r.bottom = r.top + 256;

			CWindowDC dc ( this );
			static DWORD lastBlitTime = 0;
			
			DWORD elapsedTicks = time - lastBlitTime;
//			if ( elapsedTicks > ( 1000 / 50 ) )
			{
				lastBlitTime = time;

				CSize magnifiedArea (120,120 );

				CRect rSrc, rDst;

				if ( this->m_checkMagnifyCenter.GetCheck() )
				{
					// draw the top band
					rSrc.SetRect ( CPoint(0,0), CPoint(256,128 - magnifiedArea.cy / 2 - 1) );
					rDst.SetRect ( r.TopLeft(), r.TopLeft() + rSrc.Size() );
					gOffscreenBuffer.Draw ( &dc, rSrc, rDst );

					// draw the bottom band
					rSrc.OffsetRect ( 0, rSrc.Height() + magnifiedArea.cy + 2 );
					rDst.OffsetRect ( 0, rDst.Height() + magnifiedArea.cy + 2);
					gOffscreenBuffer.Draw ( &dc, rSrc, rDst );

					// draw the middle band
					rSrc.SetRect ( 0, 128 - magnifiedArea.cy / 2 - 1, 128 - magnifiedArea.cx / 2 - 1, 128 + magnifiedArea.cy / 2 + 1);				
					rDst = rSrc;
					rDst.OffsetRect ( r.left,r.top );
					gOffscreenBuffer.Draw ( &dc, rSrc, rDst );

					rSrc.OffsetRect ( rSrc.Width() + magnifiedArea.cx + 2, 0 );
					rDst.OffsetRect ( rDst.Width() + magnifiedArea.cx + 2, 0 );
					gOffscreenBuffer.Draw ( &dc, rSrc, rDst );

					rSrc.SetRect ( 128, 128, 128, 128 );
					rDst.SetRect ( (r.left + r.right) / 2, (r.top + r.bottom) / 2, (r.left + r.right) / 2, (r.top + r.bottom) / 2 );

					rSrc.InflateRect ( magnifiedArea.cx / 6, magnifiedArea.cy / 6 );
					rDst.InflateRect ( magnifiedArea.cx / 2, magnifiedArea.cy / 2 );
					gOffscreenBuffer.Draw ( &dc, rSrc, rDst );

					rDst.SetRect ( 128 - magnifiedArea.cx / 2, 128 - magnifiedArea.cy / 2,
						 128 + magnifiedArea.cx / 2, 128 + magnifiedArea.cy / 2 );
					rDst.OffsetRect ( r.TopLeft() );

					dc.DrawFocusRect ( rDst );
				}
				else
				{
					rSrc.SetRect ( CPoint(0,0), CPoint(256,256) );
					rDst.SetRect ( r.TopLeft(), r.TopLeft() + rSrc.Size() );
					gOffscreenBuffer.Draw ( &dc, rSrc, rDst );
				}
			}
		}
	}
	bRecurse = false;
}


void CAlifeDlg :: GetControlValues ( void )
{
	gFoodProduction =	m_sliderFoodProduction.GetPos();
	gFoodValue =		m_sliderFoodValue.GetPos();
	gSpeed =			m_sliderDelay.GetPos();

//	gMortality =		m_sliderMortality.GetPos();
	gMutation =			m_sliderMutationRate.GetPos();
//	gMovePenalty =		m_sliderMovePenalty.GetPos();
//	gTurnCost =			m_sliderTurnCost.GetPos();
//	gDivideEnergy =		m_sliderDivideCost.GetPos();
//	m_bShowDisplay =	m_ctrlShowDisplay.GetCheck();
}

void CAlifeDlg::OnOK() 
{
	gRunThread = false;
//	gReporter.WriteReport();
//	fclose ( pLogFile );
	
	Sleep(500);
	CDialog::OnOK();
}

#include "Organism.h"

CString StringFromSignature ( LPCSTR pSig );
CString StringFromSignature ( LPCSTR pSig )
{
	if ( pSig == NULL )
		return "Food";

	CString str;
	while ( *pSig )
	{
		str += SegmentTypeDescriptor::GetShortString ( *pSig );
		++pSig;
	}
	return str;
}

void CAlifeDlg::UpdateCounts()
{
	CArray<int,int> arrayCounts;
	CArray<LPCSTR,LPCSTR> arraySignatures;

	arrayCounts.Add ( gBoard.GetFoodCount() );
	arraySignatures.Add ( NULL );

	CList<Organism*,Organism*> & listOrganisms = gBoard.GetListOrganisms();
/*
	if ( listOrganisms.GetCount() > 6000 )
	{
		gFoodProduction = max ( 20, gFoodProduction - 1 );
	}
	m_sliderFoodProduction.SetPos ( gFoodProduction );
*/

	int i;

// count up the species
	CMap<LPCSTR,LPCSTR,int,int> mapCounts;

	POSITION pos;

	for ( pos = listOrganisms.GetHeadPosition(); pos != NULL; )
	{
		Organism * pOrganism = listOrganisms.GetNext ( pos );
		LPCSTR pSig = pOrganism->GetSignature();

		int iCount = 0;
		if ( ! mapCounts.Lookup ( pSig, iCount ) )
		{
			int len = strlen ( pSig );
		}
		mapCounts.SetAt ( pSig, iCount + 1 );
	}
			

// now put the map into arrays
	arrayCounts.SetSize ( mapCounts.GetCount() + 1 );
	arraySignatures.SetSize ( mapCounts.GetCount() + 1 );

	i = 1;
	for ( pos = mapCounts.GetStartPosition(); pos != NULL; )
	{
		LPCSTR pSig;
		int count;

		mapCounts.GetNextAssoc ( pos, pSig, count );
		arrayCounts[i] = count;
		arraySignatures[i] = pSig;
		++i;
	}

// sort the arrays (use quicksort?)
	bool bDone = false;
	while ( ! bDone )
	{
		bDone = true;
		for ( i = 0; i < ( arrayCounts.GetSize() - 1 ); i++ )
		{
			if ( arrayCounts[i+1] > arrayCounts[i] )
			{
				int si = arrayCounts [ i ];
				arrayCounts [ i ] = arrayCounts [ i + 1 ];
				arrayCounts [ i + 1 ] = si;

				LPCSTR sps = arraySignatures [ i ];
				arraySignatures [ i ] = arraySignatures [ i + 1 ];
				arraySignatures [ i + 1 ] = sps;

				bDone = false;
			}
		}
	}

	if ( gTurnsUntilMCD == 0 )
	{
		if ( arraySignatures.GetSize() > 1 )
			if ( arraySignatures[1] )
				if ( strlen ( arraySignatures[1] ) > 1 )
					gTurnsUntilMCD = gTurns;
	}
	else
	{
		if ( arraySignatures.GetSize() > 1 )
		{
			if ( ( arraySignatures[0] && ( strlen ( arraySignatures[0] ) == 1 ) ) ||
				 ( arraySignatures[1] && ( strlen ( arraySignatures[1] ) == 1 ) ) )
				gTurnsUntilMCD = 0;
		}
	}


	CWindowDC dc ( this );
	CRect r;
	GetClientRect ( r );

	m_ctrlNames.GetWindowRect ( r );
	ScreenToClient ( r );

	int maxEntries = 10;

	r.left = m_rectDisplay.right - 5;
	r.top = r.top + 22; //bottom - 13 * maxEntries;
	r.top += 8;
	r.right = r.left + 16;
	r.bottom = r.top + 16;
	r.DeflateRect ( 4, 4 );

	CString strNames, strCounts;


	for ( i = 0; i < min ( arraySignatures.GetSize(), maxEntries ); i++ )
	{
		strNames += StringFromSignature ( arraySignatures[i] );
		strNames += '\n';

		CString strCount;
		strCount.Format ( "%d\n", arrayCounts[i] );
		strCounts += strCount;

		COLORREF color = GetColorFromSignature32 ( arraySignatures [ i ] );
		dc.FillSolidRect ( &r, color );
		r.OffsetRect ( 0, 13 );
	}

#if 0
	static CString strLastTopOrganism;
	CString strTopOrganism;

	strTopOrganism = StringFromSignature ( arraySignatures [ ( arraySignatures.GetSize() > 1 ) ? 1 : 0 ] );
	if ( strTopOrganism != strLastTopOrganism )
	{
		strLastTopOrganism = strTopOrganism;
		fprintf ( pLogFile, "Turn #%d\t", gTurns );
		for ( int i = 0; i < min ( arraySignatures.GetSize(), 4 ); i++ )
			fprintf ( pLogFile, "%s: %d\t", StringFromSignature ( arraySignatures[i] ), arrayCounts[i] );
		fprintf ( pLogFile, "\n" );

		fflush ( pLogFile );
	}
#endif

#if 0
	if ( gTurnsUntilMCD )
	{
		while ( i < maxEntries )
		{
			strNames += '\n';
			strCounts += '\n';
			++i;
		}

		strNames += "Turns Till Multicell Domination:";

		CString str;
		str.Format ( "%d", gTurnsUntilMCD );
		strCounts += str;
	}
#endif

	m_ctrlNames.SetWindowText ( strNames );
	m_ctrlCounts.SetWindowText ( strCounts );

#if 0
	if ( listOrganisms.GetCount() > 9000 )
	{
		gTurnCost++;
		m_sliderTurnCost.SetPos ( gTurnCost );
	}
#endif
}

void CAlifeDlg::OnReset() 
{
	gResetHit = 1;
}

void CAlifeDlg::OnResetBarriers1() 
{
	gResetHit = 2;
}

void CAlifeDlg::OnResetBarriers2() 
{
	gResetHit = 3;
}

void CAlifeDlg::OnResetBarriers3() 
{
	gResetHit = 4;
}

void CAlifeDlg::OnResetBarriers4() 
{
	gResetHit = 4;
}

void CAlifeDlg::DoReset1() 
{
	gBoard.Reset( 0 );	
	gFoodProduction = 50;

	extern void ResetColorSigMap();
	ResetColorSigMap();

	m_sliderMutationRate.SetPos ( 0 );
	m_sliderDelay.SetPos ( 0 );
	m_sliderFoodProduction.SetPos ( gFoodProduction );
	GetControlValues();
	Invalidate();
}

void CAlifeDlg::DoReset2() 
{
//	gBoard.Reset( 0 );	

	for ( int x = 32; x < 224; x += 1 )
		if ( ( x < 126 ) || ( x > 130 ) )
		{
			gBoard.SetGridElem ( Point16 ( x, x ), kPoison, 0, NULL );
			gBoard.SetGridElem ( Point16 ( 256 - x, x ), kPoison, 0, NULL );
		}
}

void CAlifeDlg::DoReset3() 
{
	for ( int x = 0; x < 255; x++ )
		for ( int y = 0; y < 255; y++ )
		{
			WORD w = gBoard.GetTypeAt ( Point16 ( x, y ) );
			if ( ( w == kObstacle ) || ( w == kPoison ) )
				gBoard.SetGridElem ( Point16 ( x, y ), kBlank, 0, NULL );
		}
}

void CAlifeDlg::DoReset4() 
{
	gBoard.Reset( 4 );	
	gFoodProduction = 50;
	m_sliderFoodProduction.SetPos ( gFoodProduction );
}

#include "InsertDlg.h"

void CAlifeDlg::OnInsert() 
{
	CSuspendBoardBlock sbb;

	CInsertDlg dlg;
	if ( dlg.DoModal() == IDOK )
	{
		int i;
		bool bValid = true;

		CString strCode = dlg.m_strCode;
		for ( i = 0; i < strCode.GetLength(); i++ )
			if ( SegmentTypeDescriptor :: GetFromShortString ( CString ( strCode[i] ) ) == 0 )
			{
				bValid = false;
				CString strMessage;
				strMessage.Format ( "Error: unknown instruction '%c'.", strCode[i] );
				AfxMessageBox ( strMessage );
				break;
			}

		if ( bValid )
			for ( i = 0; i < 1; i++ )
				gBoard.Add ( new Organism ( dlg.m_strCode ) );
	}
}

static bool gLButtonDown = false;
static bool gRButtonDown = false;
static CPoint gPointLast;

void CAlifeDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	return;

#if 1
#if 0
	for ( int x = 32; x < 224; x += 1 )
		if ( ( x < 126 ) || ( x > 130 ) )
		{
			gBoard.SetGridElem ( Point16 ( x, x ), kPoison, 0, NULL );
			gBoard.SetGridElem ( Point16 ( 256 - x, x ), kPoison, 0, NULL );
		}

#if 0
	for ( int x = 32; x < 256; x += 32 )
		for ( int y = 0; y < ( 256 - 32 ); y++ )
		{
			gBoard.SetGridElem ( Point16 ( x, y & 255 ), kPoison, 0, NULL );
		}
	

	for ( i = 0; i < ( y - 128 ); i++ )
			SetGridElem ( Point16 ( Random() & 255, y ), kObstacle, 0, NULL );
#else
	if ( m_rectDisplay.PtInRect ( point ) )
	{
		gLButtonDown = true;
		gPointLast = CPoint ( point.x + 4, point.y + 12 );
		SetCapture();
		OnMouseMove ( nFlags, point );
	}
#endif
#endif
#endif

	CDialog::OnLButtonDown(nFlags, point);
}


void CAlifeDlg::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if ( m_rectDisplay.PtInRect ( point ) )
	{
		gRButtonDown = true;
		gPointLast = CPoint ( point.x + 4, point.y + 12 );
		SetCapture();
		OnMouseMove ( nFlags, point );
	}
	
	CDialog::OnRButtonDown(nFlags, point);
}

void CAlifeDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if ( gLButtonDown || gRButtonDown)
	{
		point.x += 4;
		point.y += 12;
		CPoint origPoint = point;
		do
		{
			int x = ( point.x - m_rectDisplay.left );
			int y = ( point.y - m_rectDisplay.top );

			if ( ( x >= 0 ) && ( x <= 255 ) && ( y >= 0 ) && ( y <= 255 ) )
			{
				y = 255 - y;
				gBoard.SetGridElem ( Point16 ( x, y ), gLButtonDown ? kPoison /*kObstacle*/ : kBlank, 0, NULL );
			}

			if ( point.x < gPointLast.x )
				++point.x;
			else
				if ( point.x > gPointLast.x )
					--point.x;

			if ( point.y < gPointLast.y )
				++point.y;
			else
				if ( point.y > gPointLast.y )
					--point.y;

		} while ( point != gPointLast );
		gPointLast = origPoint;

	}
	CDialog::OnMouseMove(nFlags, point);
}

void CAlifeDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	gLButtonDown = false;
	ReleaseCapture();
	CDialog::OnLButtonUp(nFlags, point);
}

void CAlifeDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
	gRButtonDown = false;
	ReleaseCapture();
	CDialog::OnRButtonUp(nFlags, point);
}


void CAlifeDlg::OnSave() 
{
	CSuspendBoardBlock sbb;
	CFileDialog fileDlg ( false, "critters" );
	fileDlg.m_ofn.lpstrDefExt = "critters";
	fileDlg.m_ofn.lpstrFilter = "critters";

	if ( fileDlg.DoModal() == IDOK )
	{
		CString strPath = fileDlg.GetPathName();
		CFile f;
		f.Open ( strPath, CFile::modeWrite | CFile::modeCreate );
		if ( 1 )
		{
			CArchive ar ( &f, CArchive::store );
			gBoard.Serialize ( ar );
		}
	}
}

void CAlifeDlg::OnLoad() 
{
	CSuspendBoardBlock sbb;
	CFileDialog fileDlg ( true, "critters" );
	fileDlg.m_ofn.lpstrDefExt = "critters";
	fileDlg.m_ofn.lpstrFilter = "critters";
	if ( fileDlg.DoModal() == IDOK )
	{
		CString strPath = fileDlg.GetPathName();
		CFile f;
		f.Open ( strPath, CFile::modeRead );
		if ( 1 )
		{
			CArchive ar ( &f, CArchive::load );
			gBoard.Serialize ( ar );
		}
	}
	
}

void CAlifeDlg::OnNMCustomdrawSliderFoodProduction(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

#include "InstructionSetDlg.h"

void CAlifeDlg::OnBnClickedInstructionset()
{
	CSuspendBoardBlock sbb;
	CInstructionSetDlg dlg;
	dlg.DoModal();
}
