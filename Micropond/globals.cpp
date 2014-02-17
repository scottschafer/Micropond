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
#include "globals.h"
#include "constants.h"

int gFoodProduction = 50;
int gFoodValue = 50;
int gMortality = 100;
int gMutation = 40;//90;
int gMovePenalty = 60;
int gTurnCost = 10;
int gDivideEnergy = 4;

CString gStrAllowedInstructions = "MZ<>URLD15EeWH";

int gTurns;
int gTurnsUntilMCD;

WORD		gBoardPixels[kGridElement*kGridElement];
Organism *	gBoardOrganisms[kGridElement*kGridElement][32];
WORD		gTypeToPixel[256];


void InitGridElems ( void );
void InitGridElems ( void )
{
	gTypeToPixel [ kBlank ] =					kBlankPixel;
	gTypeToPixel [ kFood ] =					PixelTo16 ( RGB ( 32, 255, 32 ) );
	gTypeToPixel [ kObstacle ] =				PixelTo16 ( RGB ( 255, 255, 255 ) );
	gTypeToPixel [ kPoison ] =					PixelTo16 ( RGB ( 255, 255, 255 ) );
/*
	gTypeToPixel [ kSegTypePhotoSynthesize ] =	PixelTo16 ( RGB ( 32, 255, 32 ) );
	gTypeToPixel [ kSegTypeMoveAndEat ] =		PixelTo16 ( RGB ( 128, 0, 0 ) );
	gTypeToPixel [ kSegTypeTurnLeft ] =			PixelTo16 ( RGB ( 0, 0, 255 ) );
	gTypeToPixel [ kSegTypeTurnRight ] =		PixelTo16 ( RGB ( 0, 0, 255 ) );
	gTypeToPixel [ kSegTypeIfFood ] =			PixelTo16 ( RGB ( 255, 0, 0 ) );
	gTypeToPixel [ kSegTypeIfBlocked ] =		PixelTo16 ( RGB ( 255, 0, 0 ) );
	gTypeToPixel [ kSegTypeIfOrganism ] =		PixelTo16 ( RGB ( 255, 0, 0 ) );
	
	gTypeToPixel [ kSegTypeRest ] =				PixelTo16 ( RGB ( 32, 32, 32 ) );
	gTypeToPixel [ kSegTypeAttack ] =			PixelTo16 ( RGB ( 255, 32, 32 ) );
*/
 }
 