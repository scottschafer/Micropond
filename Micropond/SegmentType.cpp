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

// SegmentType.cpp: implementation of the SegmentType class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SegmentType.h"
#include "globals.h"

CArray<SegmentTypeDescriptor,SegmentTypeDescriptor>	SegmentTypeDescriptor :: m_arrayDescriptors;

int SegmentTypeDescriptor :: Randomize ( void )
{
	char instruction = 'M';
	if ( gStrAllowedInstructions.GetLength() )
		instruction = gStrAllowedInstructions [ Random() % gStrAllowedInstructions.GetLength() ];

	for ( int i = 0; i < m_arrayDescriptors.GetSize(); i++ )
		if ( m_arrayDescriptors[i].m_strShort == instruction ) 
		{
			GetProcPtr ( i );
			return i;
		}

	ASSERT ( FALSE );
	return _kNumBoardTypesPlusOne;
}