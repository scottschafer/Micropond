// Board.h: interface for the Board class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BOARD_H__BC37F8A9_ACDF_4EEB_8578_8C51C009DAE4__INCLUDED_)
#define AFX_BOARD_H__BC37F8A9_ACDF_4EEB_8578_8C51C009DAE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "constants.h"
#include "globals.h"
#include "Organism.h"


class Organism;
class Board  
{
public:
	Board();
	virtual ~Board();

	void	Validate ( void );

	void	SetPixelBuffer ( WORD * pBuffer ) { m_pBoardPixels = pBuffer; }

	void	TakeTurn ( void );
	void	GrowFood ( void );
	void	Reset ( int type = 0 );

	void	Serialize ( CArchive & ar );

	static	WORD	GetDrawElement( WORD pos )
	{
		return m_pBoardPixels [ pos ];
	}

	static void	SetGridElem ( WORD pos, BYTE type, BYTE orienation, Organism *pOrganism )
	{
		extern WORD		gTypeToPixel[256];

		SetGridElem ( pos, gTypeToPixel [ type ], type, orienation, pOrganism );
	}

	static void	SetGridElem ( WORD pos, Organism *pOrganism )
	{
		extern WORD		gTypeToPixel[256];

		m_pBoardOrganisms [ pos ] = pOrganism;
	}

	static void	SetGridElem ( WORD pos, WORD pixel, BYTE type, BYTE orienation, Organism *pOrganism )
	{
		extern WORD		gTypeToPixel[256];
#if 0
#ifdef _DEBUG
		Organism * pBoardOrganism = m_pBoardOrganisms[pos];
		if ( pOrganism )
		{
			extern Board gBoard;

			CList<Organism*,Organism*>	& listOrganisms = gBoard.m_listOrganisms;

			for ( POSITION p1 = listOrganisms.GetHeadPosition(); p1 != NULL; )
			{
				Organism * p = listOrganisms.GetNext ( p1 );
				if ( p != pOrganism )
					for ( int i = 0; i < p->m_nSegmentCount; i++ )
					{
						if ( WORD ( p->m_pos[i] ) == (WORD) pos )
							if ( p->IsDead() )
								;
							else
							{
								ASSERT ( p == pBoardOrganism );
							}
					}

			}
		}
		
		
		if ( ( pOrganism != NULL ) && ( pBoardOrganism != pOrganism ) && ( pBoardOrganism != NULL ) && 
				( ! pBoardOrganism->IsDead() ) )
			TRACE("");
#endif
#endif
		if ( m_pBoardTypes [ pos ] == kFood )
			--m_foodCount;

		m_pBoardPixels[ pos ] = pixel;
		m_pBoardOrganisms [ pos ] = pOrganism;
		m_pBoardTypes [ pos ] = type;

		if ( type == kFood )
			++m_foodCount;
		//m_organismsOnGrid[ ( x + y * kGridElement ) * kNumSegmentTypes + type ] = pOrganism;
	}

	static bool	IsEmpty ( WORD pos )
	{
		return m_pBoardPixels [ pos ] == kBlankPixel;
	}
	
	static BYTE GetTypeAt ( WORD pos )
	{
		return m_pBoardTypes [ pos ];
	}

	static bool	IsObstacleAt ( WORD pos )
	{
		return ( m_pBoardTypes[ pos ] == kObstacle ) ||
			( ( m_pBoardTypes[ pos ] >= kStartBlock ) && ( m_pBoardTypes[ pos ] <= kEndBlock ) );
	}

	static Organism * GetOrganismAt ( WORD pos ) 
	{
		return m_pBoardOrganisms[pos];
	}

	static WORD * GetPixels ( void ) { return m_pBoardPixels; }

	int		GetFoodCount ( void ) { return m_foodCount; }

	void	AddOrganismToDelete ( Organism * pOrganism )
	{
//		ASSERT ( pOrganism );
//		ASSERT ( m_listOrganisms.Find ( pOrganism ) );
//		ASSERT ( ! m_listOrganismsToDelete.Find ( pOrganism ) );
//		if ( ! m_listOrganismsToDelete.Find ( pOrganism ) )
//			m_listOrganismsToDelete.AddTail ( pOrganism );
	}

	void	Add ( Organism *pOrganism )
	{
//		ASSERT ( m_listOrganisms.Find ( pOrganism ) == NULL );
		m_listOrganisms.AddTail ( pOrganism );
	}

private:
	CList<Organism*,Organism*> & GetListOrganisms () { return m_listOrganisms; }


private:
// The grid
	static WORD *		m_pBoardPixels;
	static BYTE *		m_pBoardTypes;
	static Organism **	m_pBoardOrganisms;

	static int			m_foodCount;

// The list of organisms
	CList<Organism*,Organism*>	m_listOrganisms;
	CList<Organism*,Organism*>	m_listOrganismsToDelete;

	friend class CAlifeDlg;
};

extern Board gBoard;

#endif // !defined(AFX_BOARD_H__BC37F8A9_ACDF_4EEB_8578_8C51C009DAE4__INCLUDED_)
