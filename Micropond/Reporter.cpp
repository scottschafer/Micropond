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

// Reporter.cpp: implementation of the Reporter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "a_alife.h"
#include "Reporter.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Reporter::Reporter()
{
	m_time = 0;
}

Reporter::~Reporter()
{
}

void Reporter :: NewTime()
{
	++m_time;
}

void Reporter :: AddSignature ( LPCSTR pSig, int nCount /* = 1 */ )
{
return;
	CMap<int,int,int,int> *pCount;
	if ( ! m_mapHistory.Lookup ( pSig, pCount ) )
	{
		pCount = new CMap<int,int,int,int>;
		m_mapHistory [ pSig ] = pCount;
	}

	(*pCount) [ m_time ] += nCount;

//	while ( m_time >= pCount->GetSize() )
//		pCount->Add ( 0 );

//	(*pCount) [ m_time ] += nCount;
}

void Reporter :: WriteReport ( void )
{
	FILE * pFile = fopen ( "report.txt", "w" );
	ASSERT ( pFile );
	if (pFile == NULL )
		return;

	for ( POSITION pos = m_mapHistory.GetStartPosition(); pos != NULL; )
	{
		CString str;
		CMap<int,int,int,int> *pCount;
		int i;

		m_mapHistory.GetNextAssoc ( pos, str, pCount );

		int c = 0;

		for ( POSITION pos2 = pCount->GetStartPosition(); pos2 != NULL; )
		{
			int t, c2;
			pCount->GetNextAssoc ( pos2, t, c2 );
			c += c2;
		}


//		c /= pCount->GetSize();
		if ( c < 1000 )
			continue;

		fprintf ( pFile, "%s\t", str );
		for ( i = 0; i < m_time; i++ )
		{
			fprintf ( pFile, "%d\t", (*pCount)[i] );
			fflush ( pFile );
		}

		fprintf ( pFile, "\n" );
	}
}

Reporter gReporter;

