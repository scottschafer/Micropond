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

// Board.cpp: implemenValidatetation of the Board class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Board.h"
#include "Organism.h"
#include "Reporter.h"

Board		gBoard;

WORD *		Board :: m_pBoardPixels = NULL;
Organism **	Board :: m_pBoardOrganisms = NULL;
BYTE *		Board :: m_pBoardTypes = NULL;
int			Board :: m_foodCount = 0;

typedef Organism * OrganismPtr;

Board :: Board()
{
	m_foodCount = 0;
}

Board :: ~Board()
{
}

void Board :: TakeTurn ( void )
{
#if 0
	CFile f ("C:\\out.critters", CFile::modeWrite|CFile::modeCreate );
	CArchive ar ( &f, CArchive::store );
	Serialize ( ar );
#endif

	static bool bRecurse = false;
	if ( bRecurse )
		return;

	bRecurse = true;

	if ( m_pBoardPixels == NULL )
		m_pBoardPixels = new WORD [ kGridElement * kGridElement ];

	if ( m_pBoardTypes == NULL )
		m_pBoardTypes = new BYTE [ kGridElement * kGridElement ];

	++gTurns;

	GrowFood();

	POSITION pos;

	Validate();
	for ( pos = m_listOrganisms.GetHeadPosition(); pos != NULL; )
	{
		POSITION savePos = pos;
		Organism *pOrganism = m_listOrganisms.GetNext ( pos );
		pOrganism->TakeTurn();

		if ( pOrganism->IsDead() )
		{
			m_listOrganisms.RemoveAt ( savePos );
			pOrganism->m_bNeedsUpdate = true;
			pOrganism->UpdateBoard ( true );
			pos = m_listOrganisms.GetHeadPosition();
		}
	}

	for ( pos = m_listOrganisms.GetHeadPosition(); pos != NULL; )
	{
		Organism *pOrganism = m_listOrganisms.GetNext ( pos );
		if ( pOrganism->CanDivide() )
		{
			Validate();
			Organism *pNew = pOrganism->Divide();
			ASSERT_VALID ( pNew );
			//pNew->AssertValid();
			//pNew->m_bNeedsUpdate = true;
			//pNew->UpdateBoard();

			if ( pNew )
			{
				Add ( pNew );
				pos = m_listOrganisms.GetHeadPosition();
				Validate();
			}
		}
	}
	bRecurse = false;
}

void Board :: GrowFood()
{
	int e = 1000;

	int speed = m_foodCount * gFoodProduction / 10000;
	if ( speed < 1 )
		speed = 1;

	if ( speed > e / 2 )
		speed = e / 2;
	
	Point16 p;

	while ( speed-- )
	if ( m_foodCount < 256 * 256 * 2 / 9 )
	{
		WORD offset = ( Random() & 1 ) ? 1 : 0xFFFF;

		p.Randomize();
		while ( gBoard.GetTypeAt ( p ) != kBlank )
			p.Randomize();// = Point16 ( WORD( WORD(p) + offset ) );

		gBoard.SetGridElem ( p, kFood, 0, 0 );
	}
}

typedef Organism * OrganismPtr;

#define kGridElement 256

void Board :: Reset ( int type )
{
	gTurns = gTurnsUntilMCD = 0;

	extern void InitGridElems ( void );
	InitGridElems();

	extern void InitTypes ( void );
	InitTypes();


	if ( m_pBoardPixels == NULL )
		m_pBoardPixels = new WORD [ kGridElement * kGridElement ];

	if ( m_pBoardTypes == NULL )
		m_pBoardTypes = new BYTE [ kGridElement * kGridElement ];

	if ( m_pBoardOrganisms == NULL )
		m_pBoardOrganisms = new OrganismPtr [ kGridElement * kGridElement ];

	POSITION pos;
	for ( pos = m_listOrganisms.GetHeadPosition(); pos != NULL; )
	{
		Organism *pOrganism = m_listOrganisms.GetNext ( pos );
		delete pOrganism;
	}
	m_listOrganisms.RemoveAll();
	
	extern int nCount;
	nCount = 0;

	
	memset ( m_pBoardTypes, 0, kGridElement * kGridElement * sizeof ( BYTE ) );
	memset ( m_pBoardOrganisms, 0, kGridElement * kGridElement * sizeof ( Organism*) );

	int i;
	for ( i = 0; i < ( kGridElement * kGridElement ); i++ )
		m_pBoardPixels[i] = kBlankPixel;

	switch ( type )
	{
	case 0:
		{
		}
		break;

	case 1:
	{
		for ( int y = 0; y < 255; y++ )
		{
			SetGridElem ( Point16 ( 0, y ), kObstacle, 0, NULL );
			SetGridElem ( Point16 ( y, 0 ), kObstacle, 0, NULL );
		}
	}
		break;

	case 2:
	{
		for ( int y = 255; y > 128; y-- )
			for ( i = 0; i < ( y - 128 ); i++ )
				SetGridElem ( Point16 ( Random() & 255, y ), kObstacle, 0, NULL );
	}
		break;

	case 3:
	{
		int x = 128;
		int y = 128 - 64 - 8;

		int s = 160;
		for ( int j = 0; j < s; j++ )
		{
			SetGridElem ( Point16( x - ( s / 2 ), y + j ), kObstacle, 0, NULL );
			SetGridElem ( Point16( x - ( s / 2 ) + s, y + j ), kObstacle, 0, NULL );
			SetGridElem ( Point16( x - ( s / 2 ) + j, y + s ), kObstacle, 0, NULL );
			SetGridElem ( Point16( x - ( s / 2 ) + j, y ), kObstacle, 0, NULL );
		}

		SetGridElem ( Point16( x, y + ( ( Random() & 1 ) ? 0 : s ) ), kBlank, 0, NULL );
	}
		break;

	case 4:
		for ( i = 0; i < 32; i++ )
		{
			int x = i * 32;
			int y = i * 8;

			int s = 10 + ( Random() % 20 );
			for ( int j = 0; j < s; j++ )
			{
				SetGridElem ( Point16( x - ( s / 2 ), y + j ), kObstacle, 0, NULL );
				SetGridElem ( Point16( x - ( s / 2 ) + s, y + j ), kObstacle, 0, NULL );
				SetGridElem ( Point16( x - ( s / 2 ) + j, y + s ), kObstacle, 0, NULL );
				SetGridElem ( Point16( x - ( s / 2 ) + j, y ), kObstacle, 0, NULL );
			}

			SetGridElem ( Point16( x, y + ( ( Random() & 1 ) ? 0 : s ) ), kBlank, 0, NULL );
		}
		break;
	}

// put some food out
	m_foodCount = 0;

	for ( i = 0; i < 10000; i++ )
	{
		Point16 p;

		do
		{
			p.Randomize();
		}
		while ( ! IsEmpty ( p ) );

		gBoard.SetGridElem ( p, kFood, 0, 0 );
		ASSERT ( gBoard.GetTypeAt ( p ) == kFood );
	}

#if 0
// put out some organisms
	for ( i = 0; i < 100; i++ )
	{
//		Add ( new Organism );
		Add ( new Organism ( "M" ) );
//		Add ( new Organism ( "4LMZZZ1" ) );
//		Add ( new Organism ( "AfHMWLMTHAU" ) );
//		Add ( new Organism ( "1HMWL" ) );

//		Add ( new Organism ( "M3L" ) );
//		Add ( new Organism ( "ZfMMWoMMMAWL" ) );
	}	
#endif

//	Add ( new Organism ( "1Me<WZ"));
//	Add ( new Organism ( "M4W"));
//	Add ( new Organism ( "1Me<eZM"));
//	Add ( new Organism ( "1Me>WZZ"));
//	Add ( new Organism ( "M<1M1M1M"));
//	Add ( new Organism ( "1He<ZeM"));
//	Add ( new Organism ( ">M<W1M3"));	
//	Add ( new Organism ( ">Z0HeMMMMW6M" ) );

	for ( pos = m_listOrganisms.GetHeadPosition(); pos != NULL; )
	{
		Organism *pOrganism = m_listOrganisms.GetNext ( pos );
		pOrganism->UpdateBoard();
	}
}

void Board :: Serialize ( CArchive & ar )
{
	CArray<WORD,WORD> arrayBarriers;
	CStringArray arrayOrganisms;

	if ( ar.IsStoring() )
	{
		ar << DWORD ( 1 );
		ar << DWORD ( gFoodProduction );
		ar << DWORD ( gFoodValue );
		ar << DWORD ( gMortality );
		ar << DWORD ( gMutation );
		ar << DWORD ( gMovePenalty );
		ar << DWORD ( gTurnCost );
		ar << DWORD ( gDivideEnergy );
		ar << DWORD ( gFoodProduction );
		ar << DWORD ( gTurns );
		ar << DWORD ( gTurnsUntilMCD );

		for ( int x = 0; x < 256; x++ )
		{
			for ( int y = 0; y < 256; y++ )
			{
				if ( GetTypeAt ( Point16(x,y) ) == kObstacle )
					arrayBarriers.Add ( Point16(x,y) );
			}
		}

		arrayBarriers.Serialize ( ar );

		POSITION pos;
		extern CString StringFromSignature ( LPCSTR pSig );

		for ( pos = m_listOrganisms.GetHeadPosition(); pos != NULL; )
			arrayOrganisms.Add ( StringFromSignature ( m_listOrganisms.GetNext(pos)->GetSignature() ) );
		arrayOrganisms.Serialize ( ar );
	}
	else
	{
		DWORD version;
		ar >> version;
		ar >> (DWORD&) ( gFoodProduction );
		ar >> (DWORD&) ( gFoodValue );
		ar >> (DWORD&) ( gMortality );
		ar >> (DWORD&) ( gMutation );
		ar >> (DWORD&) ( gMovePenalty );
		ar >> (DWORD&) ( gTurnCost );
		ar >> (DWORD&) ( gDivideEnergy );
		ar >> (DWORD&) ( gFoodProduction );
		ar >> (DWORD&) ( gTurns );
		ar >> (DWORD&) ( gTurnsUntilMCD );
		arrayBarriers.Serialize ( ar );
		arrayOrganisms.Serialize ( ar );

		Reset(0);

		int i;
		for ( i = 0; i < arrayBarriers.GetSize(); i++ )
			SetGridElem ( arrayBarriers[i], kObstacle, 0, NULL );

		for ( i = 0; i < arrayOrganisms.GetSize(); i++ )
			Add ( new Organism ( arrayOrganisms[i] ) );
	}
}



void Board :: Validate ()
{
#ifdef _DEBUG
	for ( POSITION pos2 = m_listOrganisms.GetHeadPosition(); pos2 != NULL; )
		ASSERT_VALID ( m_listOrganisms.GetNext ( pos2 ) );
#endif
}