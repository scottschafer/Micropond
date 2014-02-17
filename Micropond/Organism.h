// Organism.h: interface for the Organism class.
//
//////////////////////////////////////////////////////////////////////
#if !defined(AFX_ORGANISM_H__42F20722_F558_4DDA_A55A_4C03C298FA4E__INCLUDED_)
#define AFX_ORGANISM_H__42F20722_F558_4DDA_A55A_4C03C298FA4E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "constants.h"
#include "Point16.h"
#include "Orientation.h"
#include "SegmentType.h"

WORD	GetColorFromSignature ( LPCSTR pSig );
DWORD	GetColorFromSignature32 ( LPCSTR pSig );


class Organism  
#ifdef _DEBUG
	: public CObject
#endif
{
public:
	Organism( LPCSTR pSource = NULL );
	Organism ( const Organism & src );
	void	Initialize ( void );

	~Organism();
	Organism & operator = ( const Organism & src );

	void	TakeTurn ( void );
	void	UpdateBoard( bool bEraseIfDead = false );

	Organism * Divide ( void );
	Organism * Divide ( WORD pos );
	bool	CanDivide ( void );
	int		Die ( bool bDecompose = false );
	int		GetEnergy ( void ) { return m_energy; }
	bool	IsDead ( void ) { return ( m_lifeSpan <= 0 ) || ( m_energy <= 0 ); }

	int		GetDivideEnergy ( void )
	{
		return kBaseEnergy + gDivideEnergy * ( m_nSegmentCount ) * kBaseEnergy / 8;
	}

	int		GetSegmentCount ( void ) { return m_nSegmentCount; }

	Point16	GetHeadDirection ( void ) { return m_orientation; }
	void	NextInstruction ( void ) { m_iCurrentInstruction = m_pSequence1 [ m_iCurrentInstruction ]; ASSERT ( m_iCurrentInstruction < m_nSegmentCount ); }
	void	AlternateInstruction ( void ) { m_iCurrentInstruction = m_pSequence2 [ m_iCurrentInstruction ]; ASSERT ( m_iCurrentInstruction < m_nSegmentCount ); }
	int		GetCurrentInstructionIndex ( void ) { return m_iCurrentInstruction; }
	void	SetCurrentInstructionIndex ( int i ) { ASSERT ( ( i >= 0 ) && ( i < m_nSegmentCount ) ); m_iCurrentInstruction = i; }

// Performing commands
	LPCSTR	GetSignature ( void ) { return (LPCSTR) m_pSegmentArray; }

	int		GetVisionDistance ( void ) { return 30; }//15 + 5 * m_iCurrentInstruction; }
#ifdef _DEBUG
	void	AssertValid() const;
#endif
	
private:
	int					m_iCurrentInstruction;

public:
	bool				m_bAlive;
	int					m_nTotalTurns;
	OrganismProcPtr *	m_pInstructions;
	int				*	m_pSequence1;
	int				*	m_pSequence2;

	SegmentType *		m_pSegmentArray;
	int					m_nSegmentCount;

	int					m_nIfCount;
	Point16 *			m_pos;
	Orientation			m_orientation;
	WORD				m_pixel;

	int					m_lifeSpan;
	int					m_energy;
	int					m_nSleepCount;
	int					m_nStationaryTurns;

	int					m_nSight;

	bool				m_bBlocked;
	bool				m_bBlocking;

	int					m_blockCount;
	int					m_nLeaveFoodCount;
	int					m_nInstructionsPerTurn;
	BYTE				m_override;

	bool				m_bNeedsUpdate;
	bool				m_bHasMoveProc;
// conditions
		
};

typedef CList<Organism*,Organism*> OrganismList;

#endif // !defined(AFX_ORGANISM_H__42F20722_F558_4DDA_A55A_4C03C298FA4E__INCLUDED_)
