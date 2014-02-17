#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include "constants.h"

extern int gFoodProduction;
extern int gFoodValue;
extern int gMortality;
extern int gMutation;
extern int gMovePenalty;
extern int gTurnCost;
extern int gDivideEnergy;

extern int gTurns;
extern int gTurnsUntilMCD;

extern CString gStrAllowedInstructions;

class Organism;

#define RED_MASK16 		(0x1F)
#define GREEN_MASK16 	(0x1F<<5)
#define BLUE_MASK16 	(0x1F<<10)
inline WORD	PixelTo16 ( COLORREF rgb )  // This version produces a 16-bit pixel from a 24-pixel
		{	return WORD ( ( ( rgb << 7 ) & BLUE_MASK16 ) | 	// red
					( ( rgb >> 6 ) & GREEN_MASK16 ) | 	// green
					( ( rgb >> 19 ) & RED_MASK16 ) ); } 	// blue


#endif