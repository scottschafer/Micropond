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

// Organism.cpp: implementation of the Organism class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Organism.h"
#include "Board.h"
#include "globals.h"
#include "constants.h"

#define TEST 0
#define ALLOW_ELSE 1
#define ALLOW_BACKTRACK 0
#define ALLOW_POISON 0
#define ALLOW_LEAVE_FOOD 0
#define kNumStationaryTurnsIsFood 20

#define ALLOW_MUTATION !TEST
//#define ALLOW_MUTATION 0
#define ALLOW_CARNIVORES 0
#define ALLOW_BLOCK 0
#define ATTACK_ONLY_FOREIGN_SPECIES	0

// sss: the following line causes hangs!
//#define MUST_DIVIDE_ON_EMPTY 0
#define kDefaultSight 20

void NewSegmentArray ( SegmentType * pSource, SegmentType *& pResultType, OrganismProcPtr *& pResultProcs, int * & pSequence1, int * & pSequence2 );
void DisposeSegmentArray ( SegmentType * pArray );
int GetLength ( SegmentType *pSource );

#define IS_IF(x) ( (x==IfProc0) || (x==IfProc1) || (x==IfProc2) || (x==IfProc3) || (x==IfProc4) || (x==IfProc5) || (x==IfProc6) || (x==IfProc7) || (x==IfProc8) || (x==IfProc9) )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// Instructions:
int nCount = 0;

static void DoElse ( Organism * );

static void MoveProc ( Organism * );		// M	Move and eat			3
static void RestProc ( Organism * );		// Z	Rest					5
static void AttackProc ( Organism * );		// A	Attack					3
static void TurnLeftProc ( Organism * );	// <	Turn 90 degrees left	1
static void TurnRightProc ( Organism * );	// >	Turn 90 degrees right	1
static void UpProc ( Organism * );			// U	Face up					3
static void DownProc ( Organism * );		// D	Face down				3
static void RightProc ( Organism * );		// R	Face right				3
static void LeftProc ( Organism * );		// L	Face left				3
static void PoisonProc ( Organism * pOrganism );

static void IfProc1 ( Organism * );			// 12345	If statements		1
static void IfProc2 ( Organism * );
static void IfProc3 ( Organism * );
static void IfProc4 ( Organism * );
static void IfProc5 ( Organism * );
static void IfProc6 ( Organism * );			// 12345	If statements		1
static void IfProc7 ( Organism * );
static void IfProc8 ( Organism * );
static void IfProc9 ( Organism * );
static void IfProc0 ( Organism * );

static void WhileProc ( Organism * );		// W	While					1
static void EndIfProc ( Organism * );		// E	End If					1
static void ElseIfProc ( Organism * );		// e	Else If					1

static void HyperProc ( Organism * );		// H	Go hyper!				1
static void ToggleSightProc ( Organism * );	// S	Toggle sight			1
static void MarkHeadPosition ( Organism * );// *	Mark head				1
static void RestartProc ( Organism * );		// r	Start at top			1
static void LeaveBlockProc ( Organism * pOrganism );
static void EatBlockProc ( Organism * pOrganism );
static void PhotoSynthesizeProc ( Organism * pOrganism );

void InitTypes ( void );
void InitTypes ( void )
{
	static bool bInited = false;
	if ( bInited )
		return;
	bInited = true;

	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "!b",	"Blank",		NULL ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "!f",	"Food",			NULL ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "!o",	"Obstacle",		NULL ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "!p",	"Poison",		NULL ) );

	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "M",	"Move",			MoveProc ) );
//	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "~",	"Photo",		PhotoSynthesizeProc ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "Z",	"Pause",		RestProc ) );

	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "A",	"Attack",		AttackProc ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "<",	"Turn Left",	TurnLeftProc ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( ">",	"Turn Right",	TurnRightProc ) );

	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "U",	"Up",			UpProc ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "D",	"Down",			DownProc ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "R",	"Right",		RightProc ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "L",	"Left",			LeftProc ) );

#if ALLOW_POISON
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "P",	"Poison",		PoisonProc ) );
#endif

	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "0",	"IfProc0",		IfProc0 ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "1",	"IfProc1",		IfProc1 ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "2",	"IfProc2",		IfProc2 ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "3",	"IfProc3",		IfProc3 ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "4",	"IfProc4",		IfProc4 ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "5",	"IfProc5",		IfProc5 ) );

	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "6",	"IfProc6",		IfProc6 ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "7",	"IfProc7",		IfProc7 ) );

	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "8",	"IfProc8",		IfProc8 ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "9",	"IfProc9",		IfProc9 ) );

	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "E",	"End If",		EndIfProc ) );

	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "e",	"Else If",		ElseIfProc ) );

	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "W",	"While",		WhileProc ) );

	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "H",	"Hyper!",		HyperProc ) );

//	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "S",	"Toggle Sight",	ToggleSightProc ) );

	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "*",	"Mark Head",	MarkHeadPosition ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "r",	"Start at top",	RestartProc ) );

#if ALLOW_BLOCK
	//SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "E",	"Eat Block",	EatBlockProc ) );
	SegmentTypeDescriptor :: Register ( SegmentTypeDescriptor ( "B",	"Leave Block",	LeaveBlockProc ) );
#endif
}

void Organism :: Initialize ( void )
{
	static bool bInitedTypes = false;
	if ( ! bInitedTypes )
	{
		InitTypes();
		bInitedTypes = true;
	}

	m_bAlive = true;

	m_bHasMoveProc = false;
	m_lifeSpan = gMortality * gMortality + 100;
	m_iCurrentInstruction = 0;
	m_nSegmentCount = GetLength ( m_pSegmentArray );
	m_pixel = GetColorFromSignature ( (LPCSTR) m_pSegmentArray );
	m_nSleepCount = 0;
	m_nInstructionsPerTurn = 1;

	m_bBlocked = true;
	m_bBlocking = false;
	m_nSight = kDefaultSight;

	m_blockCount = 10;
	m_nLeaveFoodCount = 0;
	m_nIfCount = 0;
	m_bNeedsUpdate = true;
	m_energy = Random() % GetDivideEnergy();
	m_nStationaryTurns = 0;
	m_nTotalTurns = 0;
}

Organism::Organism( LPCSTR pSource /*= NULL */ )
{
	m_bAlive = true;

	m_pSegmentArray = NULL;
	m_pos = NULL;
	m_nStationaryTurns = 0;

	Point16 loc;
	loc.Randomize();
	loc = Point16(129,129);
	const char * kDefault = "~";//"MMM";//"~";//~";
	if ( pSource == NULL )
		pSource = kDefault;

	CArray<SegmentType,SegmentType> arraySegments;
	while ( *pSource )
	{
		char s[2];
		s[0] = *pSource++;
		s[1] = 0;

		SegmentType type = SegmentTypeDescriptor :: GetFromShortString ( s );
		if ( type )
			arraySegments.Add ( type );
	}

	arraySegments.Add ( 0 );
	NewSegmentArray ( arraySegments.GetData(), m_pSegmentArray, m_pInstructions, m_pSequence1, m_pSequence2 );

	int len = GetLength ( m_pSegmentArray );
	m_pos = new Point16 [ len ];

	for ( int i = 0; i < len; i++ )
		m_pos [ i ] = loc;

	m_orientation.Randomize();

	Initialize();
	//m_energy = 1;
}

Organism :: Organism ( const Organism & src )
{
	m_bAlive = true;

	m_lifeSpan = gMortality * gMortality + 100;
	m_iCurrentInstruction = 0;
	m_nSleepCount = 0;
	m_nInstructionsPerTurn = 1;
	m_nTotalTurns = 0;

	m_bBlocked = true;
	m_bBlocking = false;
	m_nSight = kDefaultSight;

	m_blockCount = 10;
	m_nLeaveFoodCount = 0;
	m_nIfCount = 0;
	m_bNeedsUpdate = true;
	m_energy = Random() % GetDivideEnergy();
	m_nStationaryTurns = 0;
	*this = src;
}

Organism & Organism :: operator = ( const Organism & src )
{
	NewSegmentArray ( src.m_pSegmentArray, m_pSegmentArray, m_pInstructions, m_pSequence1, m_pSequence2 );

	int len = GetLength ( m_pSegmentArray );
	ASSERT ( len > 0 );
	m_pos = new Point16 [ len ];

	for ( int i = 0; i < len; i++ )
		m_pos [ i ] = src.m_pos [ i ];

	m_orientation.Randomize();

	Initialize();

	return *this;
}

Organism::~Organism()
{
	delete[] m_pos;
	DisposeSegmentArray ( m_pSegmentArray );
}

//void Organism :: ApplyToBoard ( void )
//{
//	m_bBlocking = false;
//}


void Organism :: TakeTurn ( void )
{
	++m_nTotalTurns;
	++m_nStationaryTurns;
	extern CString StringFromSignature ( LPCSTR pSig );

	if ( m_nStationaryTurns > 2000 )
		TRACE("%s is stuck\n", StringFromSignature ( GetSignature() ) );
//	if ( IsDead() )
//		return;

//	UpdateBoard();
	ASSERT_VALID (this);

	int n = m_nInstructionsPerTurn * 2;
	m_energy -= gTurnCost * n; //max ( kEnergyCosts [ segment & 0xFF ], 0 );

	if ( IsDead() )
	{
		Die();
		//m_bNeedsUpdate = true;
		return;
	}
 
	while ( n-- )
	{
		//--m_lifeSpan;
		
		if ( m_nSleepCount )
			--m_nSleepCount;
		else
		{
			OrganismProcPtr p = m_pInstructions [ m_iCurrentInstruction ];
			ASSERT ( p );
			(p) ( this );
		}
	}
}

Organism * Organism :: Divide ( WORD ptNewLoc )
{
	int i;

	Orientation n = m_orientation;
	while ( n == m_orientation )
		n.Randomize();

	Organism *pNew = new Organism ( *this );
	pNew->m_energy = m_energy = GetDivideEnergy() / 2;

	pNew->m_orientation = n;


#if ALLOW_MUTATION

	if ( ( abs ( Random() ) % 100 ) < gMutation )
	{
		int nTimesToMutate = 1;

		while ( nTimesToMutate-- )
		{
		int len = GetLength( pNew->m_pSegmentArray );

		CArray<SegmentTypeDataType,SegmentTypeDataType> a;
		a.SetSize ( len + 1 );
		memcpy ( a.GetData(), pNew->m_pSegmentArray, ( len + 1 ) * sizeof ( SegmentTypeDataType ) );
		ASSERT ( GetLength ( (SegmentType*) a.GetData() ) == len );

		switch ( Random() % 3 )
		{
		case 0:
		{
			i = Random() % len;
			SegmentType t;
			t.Randomize();
			a[i] = t;
		}
		break;
		case 1:
		{
			i = Random() % ( len + 1 );
			SegmentType t;
			t.Randomize();
			a.InsertAt ( i, t );
		}
		break;

		case 2:
			if ( len < 2 )
				goto done;
			a.RemoveAt ( Random() % len );
			break;
		}

		ASSERT ( a [ a.GetSize() - 1 ] == 0 );

		DisposeSegmentArray ( pNew->m_pSegmentArray );

		NewSegmentArray ( (SegmentType*) a.GetData(), pNew->m_pSegmentArray, pNew->m_pInstructions, pNew->m_pSequence1, pNew->m_pSequence2 );
		pNew->m_nSegmentCount = GetLength ( pNew->m_pSegmentArray );
		
		if ( pNew->m_nSegmentCount != m_nSegmentCount )
		{
			delete [] pNew->m_pos;
			pNew->m_pos = new Point16 [ pNew->m_nSegmentCount ];
		}
	}
	}
#endif

done:
	pNew->m_nSegmentCount = GetLength ( pNew->m_pSegmentArray );
	for ( i = 0; i < pNew->m_nSegmentCount; i++ )
	{
		pNew->m_pos[i] = ptNewLoc;
		if ( pNew->m_pInstructions[i] == MoveProc )
			m_bHasMoveProc = true;
	}

	if ( gBoard.GetTypeAt ( pNew->m_pos[0] ) != kBlank )
		TRACE("");

	pNew->m_bNeedsUpdate = true;
	pNew->UpdateBoard();
	ASSERT_VALID ( pNew );

	return pNew;
}

Organism * Organism :: Divide ( void )
{
	ASSERT_VALID (this);

	ASSERT ( CanDivide() );

	Point16 pt = m_pos[0];
	Point16 ptNewLoc = pt;


	int i;

	for ( i = 0; i < 1000; i++ )
	{
		Orientation o;
		o.Randomize();
		ptNewLoc = pt + o;
		if ( gBoard.IsEmpty ( ptNewLoc ) )
			break;
	}

	ASSERT ( i != 1000 );

	if ( gBoard.IsObstacleAt ( ptNewLoc ) )
		ptNewLoc = pt;

	return Divide ( ptNewLoc );
}


bool Organism :: CanDivide ( void )
{
	if ( TEST )
		return false;

	int divideEnergy = GetDivideEnergy();
	if ( m_energy < divideEnergy )
		return false;

//#if !MUST_DIVIDE_ON_EMPTY
//	return true;
//#endif

	if ( m_bHasMoveProc )
		return false;

	UpdateBoard();
	Point16 pt ( m_pos[0] );

	if ( Board::IsEmpty ( pt + kLeftOffset ) ||
		 Board::IsEmpty ( pt + kRightOffset ) ||
		 Board::IsEmpty ( pt + kUpOffset ) ||
		 Board::IsEmpty ( pt + kDownOffset ) )
		return true;

	m_energy = 1;
	return false;
}


int Organism :: Die ( bool bDecompose )
{
	m_bAlive = false;
//	if ( GetLength ( m_pSegmentArray ) > 1 )
//		TRACE("huh?");

	int energy = m_energy;

	energy = GetDivideEnergy() / 2;
	m_energy = -kBaseEnergy;

	int len = GetLength ( m_pSegmentArray );
	for ( int i = 0; i < len; i++ )
#if 1
//		gBoard.SetGridElem ( m_pos [ i ], kFood, 0, NULL );//( true && ( ( i & 1 ) == 0 ) ) ? kFood : kBlank, 0, NULL );
		gBoard.SetGridElem ( m_pos [ i ], ( true && ( ( i & 1 ) == 0 ) ) ? kFood : kBlank, 0, NULL );
#else
		gBoard.SetGridElem ( m_pos [ i ], ( m_bDecompose ) ? kFood : kBlank, 0, NULL );
#endif

//	m_bNeedsUpdate = true;
//	UpdateBoard( true );
	extern int gFoodValue;
	return GetDivideEnergy() * gFoodValue / 100; //GetDivideEnergy() / 8;
}


CMap<CString,LPCSTR, LPCSTR,LPCSTR>						mapTypeToUniqueSegmentArray;
CMap<CString,LPCSTR, OrganismProcPtr*,OrganismProcPtr*> mapTypeToUniqueOrganismProcArray;
CMap<CString,LPCSTR, int*,int*>							mapTypeToUniqueSequence1;
CMap<CString,LPCSTR, int*,int*>							mapTypeToUniqueSequence2;

void FormSequenceArray ( OrganismProcPtr * pProcPtr, int len, int * pSequence, bool bIf );

void DisposeSegmentArray ( SegmentType * pArray )
{
//	delete [] pArray;
}

int GetLength ( SegmentType *pSource )
{
	int len = 0;
	while ( *pSource )
	{
//		ASSERT ( (*pSource & 0xFF) <= kNumSegmentTypes );
		++pSource;
		++len;
	}
	return len;
}

WORD	GetColorFromSignature ( LPCSTR pSig )
{
	return PixelTo16 ( GetColorFromSignature32 ( pSig ) );
}

////////////////////////////
static void MoveProc ( Organism * pOrganism )
{
gBoard.Validate();

	pOrganism->NextInstruction();
	pOrganism->m_energy -= gMovePenalty;// * 5;
//	if ( pOrganism->m_nSegmentCount == 1 )
//		return;

	if ( pOrganism->IsDead() )
		return;

	++pOrganism->m_nSleepCount;

	Point16 ptMoveBy = pOrganism->GetHeadDirection();
	Point16 ptNewPos = pOrganism->m_pos[0] + ptMoveBy;

	bool bObstacle = gBoard.IsObstacleAt ( ptNewPos );
	BYTE newPosType = gBoard.GetTypeAt ( ptNewPos );


	if ( newPosType == kPoison )
		TRACE("");

	Organism * pHitOrganism = gBoard.GetOrganismAt ( ptNewPos );
	if ( bObstacle )
		ASSERT ( pHitOrganism == NULL );

#if ALLOW_BACKTRACK
	if ( pHitOrganism == pOrganism )
	{
		newPosType = kBlank;
		bObstacle = false;
	}
#endif

	if ( pHitOrganism && ( pHitOrganism != pOrganism ) )
#if 0
		if ( ( Board::GetTypeAt ( ptNewPos ) == kFood ) ||
			( pHitOrganism->m_nStationaryTurns > kNumStationaryTurnsIsFood ) )
		{
//			gBoard.AddOrganismToDelete ( pHitOrganism );
			pOrganism->m_energy += pHitOrganism->Die();
			pHitOrganism->m_bNeedsUpdate = true;
			pHitOrganism->UpdateBoard ( true );
			pHitOrganism = NULL;
			//newPosType = kObstacle;
		}
		else
#endif
			newPosType = kObstacle;

	pOrganism->m_bBlocked = true;

#if ALLOW_POISON
	if ( newPosType == kPoison )
	{
		newPosType = kBlank;
		bObstacle = false;
		pOrganism->m_energy -= kBaseEnergy / 2;//= 0;//-= gFoodValue * 8;
	}
#endif

#if ALLOW_OVERLAP
	if ( bObstacle )
#else
	if ( ( newPosType != kFood ) && ( newPosType != kBlank ) )

#endif
	{
//		m_bHitBarrier = true;

#if 1
		//Organism *pBlockingOrganism = gBoard.GetOrganismAt ( ptNewPos );
		if ( pHitOrganism )
			pHitOrganism->m_bBlocking = true;


#endif
		return;
	}
//	m_bHitBarrier = false;

	int numSegments = pOrganism->m_nSegmentCount;

#if ALLOW_LEAVE_FOOD
	if ( pOrganism->m_nLeaveFoodCount )
	{
		--pOrganism->m_nLeaveFoodCount;
		gBoard.SetGridElem ( pOrganism->m_pos [ numSegments - 1 ], kFood, 0, 0 );
	}
	else
#endif

	int newOrientation = pOrganism->m_orientation;

	pOrganism->m_bNeedsUpdate = true;

	if ( newPosType == kFood )
	{
		pOrganism->m_energy += gFoodValue * 50;
	}

	int i;
	WORD lastPos = pOrganism->m_pos [ numSegments - 1 ];
	gBoard.SetGridElem ( lastPos, kBlank, 0, 0 );

	for ( i = 0; i < numSegments; i++ )
	{
		Point16		ptOldPos = pOrganism->m_pos[i];
		Organism * pHitOrganism = gBoard.GetOrganismAt ( ptNewPos );
//		ASSERT ( ( pHitOrganism == NULL ) || ( pHitOrganism == pOrganism ) );

		pOrganism->m_pos[i] = ptNewPos;
		ptNewPos = ptOldPos;
	}

	pOrganism->m_bNeedsUpdate = true;
	pOrganism->UpdateBoard();
	ASSERT_VALID ( pOrganism );

	pOrganism->m_bBlocked = false;
	pOrganism->m_nStationaryTurns = 0;

	if ( gBoard.GetOrganismAt ( lastPos ) == NULL )
	{
		pOrganism->m_bHasMoveProc = false;
		bool bCanDivide = pOrganism->CanDivide();
		pOrganism->m_bHasMoveProc = true;

		if ( bCanDivide )
		{
			Organism *pNew = pOrganism->Divide( lastPos );
			gBoard.Add ( pNew );
			pNew->m_bNeedsUpdate = true;
			pNew->UpdateBoard();
		}
	}
/*
	if ( pOrganism->CanDivide() )
	{
		Organism *pNew = pOrganism->Divide();
		if ( pNew )
			gBoard.Add ( pNew );
	}
*/
	gBoard.Validate();
}

static void RestProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();

	if ( pOrganism->m_nInstructionsPerTurn > 1 )
	{
		pOrganism->m_nInstructionsPerTurn = 1;
		return;
	}

	int kNumTurnsToSleep = 2;

	pOrganism->m_energy += gTurnCost * kNumTurnsToSleep * 3 / 4;
	pOrganism->m_nSleepCount += kNumTurnsToSleep;
}

static void AttackProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();

	Point16 ptMoveBy = pOrganism->GetHeadDirection();
	
	Point16 ptNewPos = pOrganism->m_pos[0];
	
	for ( int i = 0; i < 2; i++ )
	{
		ptNewPos += ptMoveBy;
	
		Organism * pAttackedOrganism = gBoard.GetOrganismAt ( ptNewPos );
		if ( pAttackedOrganism && ( pAttackedOrganism != pOrganism ) )
		{
			SegmentType * pSegmentArray1 = pOrganism->m_pSegmentArray;
			SegmentType * pSegmentArray2 = pAttackedOrganism->m_pSegmentArray;

#if ATTACK_ONLY_FOREIGN_SPECIES			
//			if ( pSegmentArray1 != pSegmentArray2 )
			if ( ( pOrganism->m_nSegmentCount != pAttackedOrganism->m_nSegmentCount ) && 
				( pOrganism->m_orientation != pAttackedOrganism->m_orientation ) )
#endif
			{
				//if ( pAttackedOrganism->m_bBlocked )
			//		return false;

				pAttackedOrganism->m_energy -= pOrganism->m_nSegmentCount * kBaseEnergy / 4;//4;
				pAttackedOrganism->m_nSleepCount += 1;
				return;
			}
		}
	}
	
	pOrganism->m_nSleepCount += 1;
//	pOrganism->m_energy -= gTurnCost;
}

static void TurnLeftProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();
	--pOrganism->m_orientation;
}

static void TurnRightProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();
	++pOrganism->m_orientation;
}

static void UpProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();
	pOrganism->m_orientation = 1;
}

static void DownProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();
	pOrganism->m_orientation = 3;
//	++pOrganism->m_nSleepCount;
}

static void LeftProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();
	pOrganism->m_orientation = 0;
//	++pOrganism->m_nSleepCount;
}

static void RightProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();
	pOrganism->m_orientation = 2;
//	++pOrganism->m_nSleepCount;
}

inline bool SeeFood ( Organism *pOrganism )
{
	int distance = pOrganism->GetVisionDistance();

	Point16 ptMoveBy = pOrganism->GetHeadDirection();
	Point16 ptNewPos = pOrganism->m_pos[0];

	while ( distance-- )
	{
		ptNewPos += ptMoveBy;

		BYTE type = Board::GetTypeAt ( ptNewPos );

		if ( type == kFood )
			return true;

		if ( type == kPoison )
			continue;

		if ( type != kBlank )
			return false;
	}

	return false;
}

inline bool SeePoison ( Organism *pOrganism )
{
	int distance = pOrganism->GetVisionDistance();

	Point16 ptMoveBy = pOrganism->GetHeadDirection();
	Point16 ptNewPos = pOrganism->m_pos[0];

	while ( distance-- )
	{
		ptNewPos += ptMoveBy;

		BYTE type = Board::GetTypeAt ( ptNewPos );

		if ( type == kPoison )
			return true;

		if ( type != kBlank )
			return false;
	}

	return false;
}

inline bool FoodInVicinity ( Organism *pOrganism )
{
	int distance = pOrganism->GetVisionDistance();

	int d = distance / 2;
	for ( int x = 0; x <= d; x++ )
		for ( int y = 0; y <= d; y++ )
		{
			Point16 pt = pOrganism->m_pos[0];
			pt += Point16(x - d/2,y - d/2);
			BYTE type = Board::GetTypeAt ( pt );
			if ( type == kFood )
				return true;
		}
	return false;
}

inline bool SeeForeignOrganism ( Organism *pOrganism )
{
	int distance = pOrganism->GetVisionDistance();

	Point16 ptMoveBy = pOrganism->GetHeadDirection();
	Point16 ptNewPos = pOrganism->m_pos[0];

	while ( distance-- )
	{
		ptNewPos += ptMoveBy;

		Organism *pDiffOrganism = Board :: GetOrganismAt ( ptNewPos );
		if ( pDiffOrganism && ( pDiffOrganism != pOrganism ) &&
//			( pDiffOrganism->m_pSegmentArray != pOrganism->m_pSegmentArray ) )
			( pDiffOrganism->m_nSegmentCount != pOrganism->m_nSegmentCount ) )
			return true;

		BYTE type = Board::GetTypeAt ( ptNewPos );

		if ( type != kBlank )
			return false;
	}

	return false;
}


static void IfProc0 ( Organism * pOrganism )
{
	bool bFlag = SeeFood ( pOrganism );

	if ( bFlag )
		pOrganism->NextInstruction();
	else
		pOrganism->AlternateInstruction();
}

static void IfProc1 ( Organism * pOrganism )
{
	bool bFlag = SeeFood ( pOrganism );

	if ( ! bFlag )
		pOrganism->NextInstruction();
	else
		pOrganism->AlternateInstruction();
}

static void IfProc2 ( Organism * pOrganism )
{
#if ALLOW_POISON
	bool bFlag = SeePoison ( pOrganism );
#else
	bool bFlag = FoodInVicinity ( pOrganism );
#endif

	
	if ( bFlag )
		pOrganism->NextInstruction();
	else
		pOrganism->AlternateInstruction();
}

static void IfProc3 ( Organism * pOrganism )
{
	bool bFlag = FoodInVicinity ( pOrganism );

	if ( !bFlag )
		pOrganism->NextInstruction();
	else
		pOrganism->AlternateInstruction();
}

static void IfProc4 ( Organism * pOrganism )
{
	bool bFlag = pOrganism->m_bBlocked;

	if ( bFlag )
		pOrganism->NextInstruction();
	else
		pOrganism->AlternateInstruction();
}

static void IfProc5 ( Organism * pOrganism )
{
	bool bFlag = pOrganism->m_bBlocked;

	if ( !bFlag )
		pOrganism->NextInstruction();
	else
		pOrganism->AlternateInstruction();
}

static void IfProc6 ( Organism * pOrganism )
{
	bool bFlag = SeeForeignOrganism ( pOrganism ); //pOrganism->LookFor ( kBlank + 1, SegmentTypeDescriptor::GetMaxType() - 1, distance );

	if ( bFlag )
		pOrganism->NextInstruction();
	else
		pOrganism->AlternateInstruction();
}

static void IfProc7 ( Organism * pOrganism )
{
	bool bFlag = SeeForeignOrganism ( pOrganism ); //pOrganism->LookFor ( kBlank + 1, SegmentTypeDescriptor::GetMaxType() - 1, distance );

	if ( ! bFlag )
		pOrganism->NextInstruction();
	else
		pOrganism->AlternateInstruction();
}

static void IfProc8 ( Organism * pOrganism )
{
	bool bFlag = pOrganism->m_bBlocking;
	pOrganism->m_bBlocking = false;

	if ( bFlag )
		pOrganism->NextInstruction();
	else
		pOrganism->AlternateInstruction();
}

static void IfProc9 ( Organism * pOrganism )
{
	bool bFlag = pOrganism->m_nTotalTurns > 20; //pOrganism->m_bBlocking;
//	pOrganism->m_bBlocking = false;

	if ( ! bFlag )
		pOrganism->NextInstruction();
	else
		pOrganism->AlternateInstruction();
}

static void EndIfProc ( Organism * pOrganism )
{
//	ASSERT (FALSE);
/*
	bool bFound = false;

	int i = pOrganism->GetCurrentInstructionIndex();
	for ( int n = 0; n < pOrganism->m_nSegmentCount; n++ )
	{
		++i;
		if ( i == pOrganism->m_nSegmentCount )
			i = pOrganism->m_iHeadInstruction;

		OrganismProcPtr proc = pOrganism->m_pInstructions [ i ];
		if ( IS_IF ( proc ) )
			break;

		if ( proc == EndIfProc )
		{
			bFound = true;
			break;
		}
	}

	if ( bFound )
		pOrganism->GetCurrentInstructionIndex() = ( i + 1 ) % pOrganism->m_nSegmentCount;
	else
		RestProc ( pOrganism );
*/
 }


static void LeaveBlockProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();

	if ( pOrganism->m_blockCount )
	{
		--pOrganism->m_blockCount;
//	pOrganism->m_nSleepCount += 2;

		int numSegments = GetLength ( pOrganism->m_pSegmentArray );
		Point16 pt = pOrganism->m_pos [ numSegments - 1 ];
		Point16 ptMoveBy = pOrganism->GetHeadDirection();

		pt -= ptMoveBy;

		gBoard.SetGridElem ( pt, kStartBlock, 0, 0 );
	}
}

static void EatBlockProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();

	Point16 ptMoveBy = pOrganism->GetHeadDirection();
	Point16 ptNewPos = pOrganism->m_pos[0] + ptMoveBy;

	BYTE newPosType = gBoard.GetTypeAt ( ptNewPos );

	if ( newPosType == kObstacle )
	{
		gBoard.SetGridElem ( ptNewPos, kBlank, 0, 0 );
		pOrganism->m_nSleepCount += 4;

//		++pOrganism->m_blockCount;
	}
}

static void TellProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();

	SegmentType s = pOrganism->m_pSegmentArray [ pOrganism->GetCurrentInstructionIndex() ];
	pOrganism->NextInstruction();
	
	Point16 ptMoveBy = pOrganism->GetHeadDirection();
	Point16 ptNewPos = pOrganism->m_pos[0];
	
	for ( int i = 0; i < 5; i++ )
	{
		ptNewPos += ptMoveBy;
		Organism *pTOrganism = gBoard.GetOrganismAt ( ptNewPos );
		if ( pTOrganism && ( pOrganism != pTOrganism ) )
		{
//			ASSERT ( pTOrganism != pOrganism );
//			if ( pTOrganism->m_pSegmentArray != pOrganism->m_pSegmentArray )
				pTOrganism->m_override = s;
		}
	}

}

static void PoisonProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();
#if 1

	if ( pOrganism->m_energy > ( pOrganism->GetDivideEnergy() * .75 ) )
	{
		pOrganism->m_energy -= gMovePenalty;// * 5;

//	if ( pOrganism->m_blockCount == 0 )
//		return false;
//	--pOrganism->m_blockCount;

	int numSegments = GetLength ( pOrganism->m_pSegmentArray );
	Point16 pt = pOrganism->m_pos [ numSegments - 1 ];
	Point16 ptMoveBy = pOrganism->GetHeadDirection();

	pt -= ptMoveBy;

	gBoard.SetGridElem ( pt, kPoison, 0, 0 );

#else
	Point16 ptMoveBy = pOrganism->GetHeadDirection();
	Point16 ptNewPos = pOrganism->m_pos[0] + ptMoveBy;

	BYTE newPosType = gBoard.GetTypeAt ( ptNewPos );

	if ( newPosType == kObstacle )
	{
		gBoard.SetGridElem ( ptNewPos, kBlank, 0, 0 );
		++pOrganism->m_blockCount;
	}
#endif
	}
}

static CMap<CString,LPCSTR, COLORREF, COLORREF> mapSigToColor;
static int colorSigIndex = 0;

void ResetColorSigMap()
{
	mapSigToColor.RemoveAll();
	colorSigIndex = 0;
}

DWORD	GetColorFromSignature32 ( LPCSTR pSig )
{
	if ( pSig == NULL )
		return RGB ( 32, 255, 32 ); // Food

	COLORREF c;
	if ( mapSigToColor.Lookup ( pSig, c ) )
		return c;

	colorSigIndex = ( colorSigIndex + 1 ) % ( sizeof ( gOrganismColors ) / sizeof ( gOrganismColors[0] ) );
	mapSigToColor [ pSig ] = gOrganismColors [ colorSigIndex ];
	return gOrganismColors [ colorSigIndex ];

/*

	int h = 0;
	while ( *pSig )
	{
		char c = *pSig++;
		h = h + h / 2 + c;
	}

	h %= sizeof ( gOrganismColors ) / sizeof ( gOrganismColors[0] );
	return gOrganismColors [ h ];
*/
}

static void HyperProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();
	pOrganism->m_nInstructionsPerTurn = 2;//, pOrganism->m_nInstructionsPerTurn + 1 );// = 3;//( pOrganism->m_nInstructionsPerTurn == 1 ) ? 2 : 1;
}

static void MoreFoodProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();
	if ( gFoodProduction < 100 )
		if ( ( Random() % 4 ) == 0 )
			++gFoodProduction;
}

static void LessFoodProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();
	if ( gFoodProduction )
		if ( ( Random() % 4 ) == 0 )
			--gFoodProduction;
}

static void ToggleSightProc ( Organism *pOrganism )
{
	pOrganism->NextInstruction();
	pOrganism->m_nSight = ( pOrganism->m_nSight == kDefaultSight ) ? ( kDefaultSight * 2 ) : kDefaultSight;
}

static void MarkHeadPosition ( Organism *pOrganism )
{
	pOrganism->NextInstruction();
//	if ( pOrganism->GetCurrentInstructionIndex() != ( pOrganism->m_nSegmentCount - 1 ) )
//		pOrganism->m_iHeadInstruction = pOrganism->GetCurrentInstructionIndex();
}

static void RestartProc ( Organism *pOrganism )
{
	pOrganism->SetCurrentInstructionIndex ( 0 );
}

static void LeaveFoodProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();
	pOrganism->m_energy -= gFoodValue * 50;
	++pOrganism->m_nLeaveFoodCount;
}

static void ElseIfProc ( Organism *pOrganism )
{
	pOrganism->NextInstruction();
	pOrganism->m_energy -= gTurnCost;
}

static void WhileProc ( Organism *pOrganism )
{
	pOrganism->NextInstruction();
	pOrganism->m_energy -= gTurnCost;
}


void NewSegmentArray ( SegmentType * pSource, SegmentType *& pResultType, OrganismProcPtr *& pResultProcs,
						int * & pSequence1, int * & pSequence2 )
{
	LPCSTR pType = NULL;
	OrganismProcPtr * pProcPtr = NULL;

	if ( mapTypeToUniqueSegmentArray.Lookup ( (LPCSTR) pSource, pType ) )
	{
		mapTypeToUniqueOrganismProcArray.Lookup ( (LPCSTR) pSource, pProcPtr );
		ASSERT ( pProcPtr );

		mapTypeToUniqueSequence1.Lookup ( (LPCSTR) pSource, pSequence1 );
		ASSERT ( pSequence1 );

		mapTypeToUniqueSequence2.Lookup ( (LPCSTR) pSource, pSequence2 );
		ASSERT ( pSequence2 );
	}
	else
	{
		int len = strlen ( LPCSTR ( pSource ) ) + 1;

		pType = new char [ len ];
		strcpy ( (char*) pType, (LPCSTR) pSource );
		mapTypeToUniqueSegmentArray [ LPCSTR ( pSource ) ] = pType;

		pProcPtr = new OrganismProcPtr [ len ];
		int i;
		for ( i = 0; i < ( len - 1 ); i++ )
			pProcPtr[i] = SegmentTypeDescriptor::GetProcPtr ( pType[i] );
		pProcPtr[i] = NULL;

		mapTypeToUniqueOrganismProcArray [ LPCSTR ( pSource ) ] = pProcPtr;
		
		pSequence1 = new int [ len ];
		pSequence2 = new int [ len ];

		FormSequenceArray ( pProcPtr, len - 1, pSequence1, true );
		FormSequenceArray ( pProcPtr, len - 1, pSequence2, false );

		mapTypeToUniqueSequence1 [ LPCSTR ( pSource ) ] = pSequence1;
		mapTypeToUniqueSequence2 [ LPCSTR ( pSource ) ] = pSequence2;
	}

	pResultType = (SegmentType*) pType;
	pResultProcs = pProcPtr;
}

/*
1	If				2 or 9
2		If			3 or 6
3			Move	4
4			Move	2
5		Else		(7)
6			Turn	2
7		While		(2)
8	Endif			(9)
9	Move			1
*/

void FormSequenceArray ( OrganismProcPtr * pProcPtr, int len, int * pSequence, bool bIf )
{
	int i;

	int headPos = 0;

	// Pass #1: determine head position
	for ( i = 0; i < len; i++ )
	{
		OrganismProcPtr p = pProcPtr[i];
		if ( p == MarkHeadPosition )
		{
			headPos = ( i + 1 ) % len;
			break;
		}
	}

	// Pass #2: set up generic sequence information
	for ( i = 0; i < len; i++ )
	{
		OrganismProcPtr p = pProcPtr[i];
		int iNext = ( i + 1 ) % len;

		if ( p == RestartProc )
			pSequence [ i ] = 0;
		else
			if ( iNext == 0 )
				pSequence [ i ] = headPos;
			else
				pSequence [ i ] = iNext;
	}

	// Pass #3:  resolve If/Else/While statements.  Go from the bottom up.
	CArray<int,int> ifEndArray;
	ifEndArray.SetSize(len);
	for ( i = 0; i < len; i++ )
		ifEndArray[i] = 0;

	//
	for ( i = len - 2; i >= headPos; i-- ) // an if at the end is pointless
	{
		bool bIsIf = IS_IF ( pProcPtr[i] );

		if ( bIsIf )
		{
			// This is an If statement.
			int iEnd = 0;
			int iElse = 0;
			int iWhile = 0;

			int x = ( i + 1 ) % len;

			while ( x < len )
			{
				OrganismProcPtr p = pProcPtr[x];
				if ( IS_IF ( p ) )
				{
					if ( ifEndArray [ x ] )
						x = ifEndArray [ x ];
				}
				else
					if ( p == ElseIfProc )
					{
#if ALLOW_ELSE
						if ( iElse != 0 )
							iEnd = x;
						else
							iElse = x;
#else
						iEnd = x;
						break;
#endif
					}
					else
						if ( p == WhileProc )
						{
							iWhile = iEnd = x;
							break;
						}
				++x;
			}

			ifEndArray [ i ] = iEnd;

			// the else statement should skip over the end, if need be
			if ( iElse && iEnd )
				pSequence [ iElse ] = ( iEnd + 1 ) % len;

			if ( bIf )
			{
				// this is the case for when the If statement is true;

				if ( iWhile )
				{
					if ( iElse )
						pSequence [ iElse ] = i;
					else
						pSequence [ iWhile ] = i;
				}
			}
			else
			{
				// this is the case for when the If statement is false
				if ( iElse )
					pSequence [ i ] = ( iElse + 1 ) % len;		// go just past Else, if present
				else
					if ( iEnd )								
						pSequence [ i ] = ( iEnd + 1 ) % len; 	// or if no Else, go to end
					else
						//pSequence [ i ] = 0; //( iEnd + 1 ) % len;
						pSequence [ i ] = (i + 2) % len;		// if all else fails, just skip one instruction

			}
		}
	}

	// pass #4
	// If any instructions have as their next instruction Else or While, skip ahead to the next instruction for speed

	if ( len > 1 )
	for ( int pass = 3; pass >=0; pass-- )
		for ( i = 0; i < len; i++ )
		{
			OrganismProcPtr p = pProcPtr[ pSequence[i] ];
			if ( ( p == ElseIfProc ) || ( p == WhileProc ) || ( p == MarkHeadPosition ) || ( p == RestartProc ) )
			{
				pSequence[i] = pSequence [ pSequence[i] ];
//				if ( pass == 0 )
//				ASSERT ( pass != 0 );
			}
		}

//	for ( i = 0; i < len; i++ )
//	{
//		TRACE ( "bIf = %s, instruction #%d, next instruction is #%d\n", bIf ? "true" : "false", i + 1, pSequence[i] + 1 );
//	}
}

static void PhotoSynthesizeProc ( Organism * pOrganism )
{
	pOrganism->NextInstruction();
	pOrganism->m_energy += gFoodProduction / 2 + gTurnCost / 2;
}

void Organism :: UpdateBoard ( bool bEraseIfDead )
{
//	ASSERT ( !bEraseIfDead );

	if ( m_bNeedsUpdate )
	{
		m_bNeedsUpdate = false;
		int i;

		int numSegments = m_nSegmentCount;

		if ( IsDead() )
		{
			Die();
		}
		else
			for ( i = 0; i < numSegments; i++ )
			{
				Point16		pt = m_pos[i];

				WORD pixel = m_pixel;
				bool bSetAsFood = ( m_pInstructions[i] == PhotoSynthesizeProc );
				if ( ! bSetAsFood )
					if ( ( numSegments > 1 ) && ( WORD(m_pos[0]) == WORD(m_pos[1] )) )
						if ( Board::GetTypeAt ( pt ) == kFood )
							bSetAsFood = true;

				if ( gBoard.GetOrganismAt ( m_pos[i] ) == NULL )
					if ( bSetAsFood )
						Board::SetGridElem ( pt, kFood, 4, this );
					else
						Board::SetGridElem ( pt, pixel,	4, 0, this );
			}
	}
}

#ifdef _DEBUG
void Organism :: AssertValid() const
{
	return;
	CObject::AssertValid();
	if ( !const_cast<Organism*>(this)->IsDead() )
	for ( int i = 0; i < m_nSegmentCount; i++ )
		ASSERT ( gBoard.GetOrganismAt ( m_pos[i] ) == this );
}
#endif
