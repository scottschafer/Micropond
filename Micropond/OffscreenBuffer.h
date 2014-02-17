// OffscreenBuffer.h: interface for the COffscreenBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OFFSCREENBUFFER_H__ABAE80F4_CF0E_410A_B76B_C5696F4FCBBF__INCLUDED_)
#define AFX_OFFSCREENBUFFER_H__ABAE80F4_CF0E_410A_B76B_C5696F4FCBBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class COffscreenBuffer  
{
public:
	COffscreenBuffer();
	virtual ~COffscreenBuffer();

	void	Allocate ( int width = 256, int height = 256, int bytesPerPixel = 2 );
	void *	GetBaseAddress ( void ) { return m_pBuffer; }

#ifdef PLAT_WINDOWS
	void	Draw ( CDC * pDC, const CRect & r );
	void	Draw ( CDC * pDC, const CRect & rSrc, const CRect &rDst );
#endif

private:

#ifdef PLAT_WINDOWS
	HBITMAP		m_hDIBSection;
	BYTE *		m_pBuffer;
	BITMAPINFO	m_bitMapInfo;
	CBitmap		m_bits;
	CRect		m_srcRect;
#endif
};

#endif // !defined(AFX_OFFSCREENBUFFER_H__ABAE80F4_CF0E_410A_B76B_C5696F4FCBBF__INCLUDED_)
