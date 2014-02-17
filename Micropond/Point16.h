// Point16.h: interface for the Point16 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_POINT16_H__C1B2F14F_2A4B_49A3_849E_B3B375F44F70__INCLUDED_)
#define AFX_POINT16_H__C1B2F14F_2A4B_49A3_849E_B3B375F44F70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define POINT_MASK MAKEWORD(kGridElement-1,kGridElement-1)

class Point16  
{
public:
	Point16() {}
	Point16( int x, int y) { m_data = MAKEWORD(x,y); }
	Point16( int x ) { m_data = x; }
	void	Randomize ( void ) { m_data = Random(); } //WORD ( ( Random() * 2 ) + ( Random() & 1 )  ); }

	//operator int() { ASSERT ( int(m_data) >= 0 ); return int(m_data); }
	operator WORD() { return m_data; }// & POINT_MASK; }
	operator int() { return m_data; } // & POINT_MASK; }

	Point16 operator -() { return ((-m_data)&0xFF00)|((-m_data)&0xFF); }
	Point16 operator += ( Point16 pt )
		{ m_data = ((int(m_data)+(int(pt)&0xFF00))&0xFF00)+((int(m_data)+(int(pt)&0x00FF))&0x00FF); return *this; }
	
	Point16 operator -= ( Point16 pt )
		{ m_data = ((int(m_data)-(int(pt)&0xFF00))&0xFF00)+((int(m_data)-(int(pt)&0x00FF))&0x00FF); return *this; }

private:

	WORD	m_data;
};

inline int operator + ( Point16 a, Point16 b )
//	{		return ((int(a)+(int(b)&0xFF00))&0xFF00)+((int(a)+(int(b)&0x00FF))&0x00FF); }
	{		return ((int(a)+(int(b)&0xFF00))&0xFF00)+((int(a)+(int(b)&0x00FF))&0x00FF); }

inline int operator - ( Point16 a, Point16 b )
	{		return ((int(a)-(int(b)&0xFF00))&0xFF00)+((int(a)-(int(b)&0x00FF))&0x00FF); }


#endif // !defined(AFX_POINT16_H__C1B2F14F_2A4B_49A3_849E_B3B375F44F70__INCLUDED_)
