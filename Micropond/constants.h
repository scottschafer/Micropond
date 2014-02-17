#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

// conditions
#define ALLOW_OVERLAP 0

// Segment types
enum
{
	kBlank = 0,
	kFood,
	kObstacle,
	kPoison,
	
	_kNumBoardTypesPlusOne,
	
	kStartBlock = 200, //254,
	kEndBlock = 255
};

#define kNumSegmentTypes (_kNumSegmentTypesPlusOne - 1)

enum
{
	kConditionSeeFood =				0x0100,
	kConditionBlocked =				0x0200,
	kConditionAttacked =			0x0400,
	kConditionSeeOrganism =			0x0800,
	kConditionSeeForeignOrganism =	0x1000,
	kMaskOutCondition = 0x00FF
};

// Board dimensions
enum
{
	kGridElement = 256,
//	kGridElement = 128,
	kSmallGridElementSize = 1,
	kLargeGridElementSize = 16,

	kMagnifyGridElements = 20
};

// Energy
enum
{
	kBaseEnergy = 10000
};

enum
{
	kMaxSegments = 32
};


#if ( kGridElement != 256 )
	inline int WrapCoordinate ( int x ) { return ( x < 0 ) ? ( kGridElement - 1 ) : ( ( x >= kGridElement ) ? 0 : x ); }
	inline void WrapPoint ( CPoint &pt ) { pt.x = WrapCoordinate ( pt.x ); pt.y = WrapCoordinate ( pt.y ); }
#else
	#define WrapCoordinate(x) x
	#define WrapPoint(x)
#endif

#define kBlankPixel 0

static const COLORREF gOrganismColors[] = 
{
#if kBlankPixel == 0xFFFF
	/*"CMD_BLACK",*/		RGB ( 0x00, 0x00, 0x00 ),
#else
	/*"CMD_BLACK",*/		RGB ( 0xFF, 0xFF, 0xFF ),
#endif
	/*"CMD_RED", */			RGB ( 0xFF, 0x00, 0x00 ),
//	/*"CMD_GREEN",*/		RGB ( 0x00, 0x80, 0x00 ),
	/*"CMD_BLUE", */		RGB ( 0x00, 0x00, 0xFF ),
	/*"CMD_LIGHT_BLUE", */	RGB ( 0x80, 0x80, 0xFF ),
	/*"CMD_PINK", */		RGB ( 0xFF, 0x00, 0xFF ),
//	/*"CMD_YELLOW", */		RGB ( 0xFF, 0xFF, 0x00 ),
	/*"CMD_MAROON", */		RGB ( 0x80, 0x00, 0x00 ),
	/*"CMD_BROWN", */		RGB ( 110,	50, 20 ),
	/*"CMD_OLIVE", */		RGB ( 0x80, 0x80, 0x00 ),
	/*"CMD_NAVY", */		RGB ( 0x00, 0x00, 0x80 ),
	/*"CMD_PURPLE", */		RGB ( 0x80, 0x00, 0x80 ),
	/*"CMD_TEAL", */		RGB ( 0x00, 0x80, 0x80 ),
	/*"CMD_GRAY", */		RGB ( 0x80, 0x80, 0x80 ),
//	/*"CMD_SILVER", */		RGB ( 0xC0, 0xC0, 0xC0 ),
//	/*"CMD_LIME", */		RGB ( 0x00, 0xFF, 0x00 ),
	/*"CMD_ORANGE", */		RGB ( 0xFF, 0x80, 0x00 ),

//	/*"CMD_FOREST_GREEN",*/	RGB (  24, 117,  52 ),
//	/*"CMD_BURNT_ORANGE",*/	RGB ( 204, 102,   0 ),
	/*"CMD_SKY", */			RGB (  93, 186, 202 ),
	/*"CMD_GLASS", */		RGB ( 153, 204, 204 ),
//	/*"CMD_TAN", */			RGB ( 190, 132,  74 ),
//	/*"CMD_TOMATO", */		RGB ( 246,  63,  27 ),
//	/*"CMD_SUNSHINE", */	RGB ( 255, 234,  24 ),
	/*"CMD_MARIGOLD", */	RGB ( 255, 175,  24 ),
	/*"CMD_LAVENDER", */	RGB ( 187,  86, 195 ),
//	/*"CMD_BLUSH", */		RGB ( 248, 123,  87 ),
	/*"CMD_VIOLET", */		RGB (  89,  24, 187 ),
//	/*"CMD_GRASS", */		RGB (   0, 153,   0 ),
//	/*"CMD_BUFF", */		RGB ( 255, 195, 143 ),
};

#endif