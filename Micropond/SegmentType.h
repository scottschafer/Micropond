// SegmentType.h: interface for the SegmentType class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SEGMENTTYPE_H__188E716A_F822_48D6_87B1_0194DFAB0817__INCLUDED_)
#define AFX_SEGMENTTYPE_H__188E716A_F822_48D6_87B1_0194DFAB0817__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "constants.h"
#include "globals.h"

typedef char SegmentTypeDataType;

class Organism;

typedef void ( * OrganismProcPtr ) ( Organism * );
extern WORD		gTypeToPixel[256];

class SegmentTypeDescriptor
{
public:
	SegmentTypeDescriptor () {}

	SegmentTypeDescriptor ( LPCSTR pShortName, LPCSTR pLongName, OrganismProcPtr proc, int likelyhood = 1 )
		: m_strShort ( pShortName ), m_strLong ( pLongName ), m_proc ( proc ), m_likelyhood ( likelyhood ) {}

//	static bool		Execute ( int i, Organism *pOrganism )	{ return m_arrayDescriptors [ i ].m_proc ( pOrganism ); }
	static CString	GetShortString  ( int i )				{ return m_arrayDescriptors [ i ].m_strShort; }
	
	static int	GetFromShortString  ( LPCSTR pType ) { 
		for ( int i = 0; i < m_arrayDescriptors.GetSize(); i++ )
			if ( m_arrayDescriptors [ i ].m_strShort == pType )
				return i;
		ASSERT ( FALSE );
		return 0;
	}
	static CString	GetLongString  ( int i )				{ return m_arrayDescriptors [ i ].m_strLong; }
	static int		Randomize ( void );
	static void		Register ( SegmentTypeDescriptor s )	{ m_arrayDescriptors.Add ( s ); }
	static OrganismProcPtr GetProcPtr ( int i )				{ return m_arrayDescriptors [ i ].m_proc; }
	static int		GetMaxType ( void )						{ return m_arrayDescriptors.GetSize(); }

private:
	CString				m_strShort;
	CString				m_strLong;
	OrganismProcPtr		m_proc;
	int					m_likelyhood;
	static CArray<SegmentTypeDescriptor,SegmentTypeDescriptor>	m_arrayDescriptors;
};

class SegmentType  
{
public:
	SegmentType() {}
	SegmentType ( int type ) { m_data = type; }

	operator SegmentTypeDataType () { return m_data; }
	void Randomize() { m_data = SegmentTypeDescriptor::Randomize(); }


private:
	SegmentTypeDataType m_data;
};

#endif // !defined(AFX_SEGMENTTYPE_H__188E716A_F822_48D6_87B1_0194DFAB0817__INCLUDED_)
