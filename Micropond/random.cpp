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

WORD Random ( void )
{
	static WORD randomWords[65536];
	static int index = 0;
	static bool bInited = false;

	if ( ! bInited )
	{		
		srand( (unsigned)time( NULL ) );

		int i;
		for ( i = 0; i < 65536; i++ )
			randomWords[i] = WORD ( i );

		for ( i = 0; i < 65536 * 4; i++ )
		{
			int i1 = ( ( rand() * 2 ) + ( rand() & 1 ) ) & 0xFFFF;
			int i2 = ( ( rand() * 2 ) + ( rand() & 1 ) ) & 0xFFFF;

			WORD s = randomWords[i1];
			randomWords[i1] = randomWords[i2];
			randomWords[i2] = s;
		}
		index = rand();
		bInited = true;
	}

	index = ( index + 1 ) & 0xFFFF;
	return randomWords [ index ];
}

