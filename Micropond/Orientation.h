// Orientation.h: interface for the Orientation class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ORIENTATION_H__DB3190ED_15B8_4879_8B02_1ADFB022BF83__INCLUDED_)
#define AFX_ORIENTATION_H__DB3190ED_15B8_4879_8B02_1ADFB022BF83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Point16.h"

#define kLeftOffset		Point16(-1,0)
#define kRightOffset	Point16(1,0)
//#define kUpOffset		Point16(0,-1)
//#define kDownOffset		Point16(0,1)
#define kUpOffset		Point16(0,1)
#define kDownOffset		Point16(0,-1)

#define kULOffset		Point16(-1,-1)
#define kDLOffset		Point16(-1,1)
#define kUROffset		Point16(1,-1)
#define kDROffset		Point16(1,-1)

#define ALLOW_DIAGONAL 0

#if ALLOW_DIAGONAL
	enum
	{
		kNumOrientations = 8
	};
	static const Point16 gOrientations[kNumOrientations] = { kLeftOffset, kULOffset, kUpOffset, kUROffset, kRightOffset, kDROffset, kDownOffset, kDLOffset };
#else
	enum
	{
		kNumOrientations = 4
	};
	static const Point16 gOrientations[kNumOrientations] = { kLeftOffset, kUpOffset, kRightOffset, kDownOffset };
#endif

class Orientation  
{
public:
	Orientation() {}
	Orientation ( int i ) { m_data = i & ( kNumOrientations - 1 ); }
	operator int()  { return m_data; }
	operator Point16 () { return gOrientations[m_data]; }
	void Randomize() { m_data = Random() & ( kNumOrientations - 1 ); }
	Orientation operator --() { m_data = ( m_data - 1 ) & ( kNumOrientations - 1 ); return *this; }
	Orientation operator ++() { m_data = ( m_data + 1 ) & ( kNumOrientations - 1 ); return *this; }

private:
	BYTE m_data;
};

#endif // !defined(AFX_ORIENTATION_H__DB3190ED_15B8_4879_8B02_1ADFB022BF83__INCLUDED_)
