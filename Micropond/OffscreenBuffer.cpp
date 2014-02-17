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
#include "a_alife.h"
#include "OffscreenBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define WIDTHBYTES(x) x

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COffscreenBuffer::COffscreenBuffer()
{

}

COffscreenBuffer::~COffscreenBuffer()
{

}

void COffscreenBuffer :: Allocate ( int width, int height, int bytesPerPixel )
{
	CRect rectBounds ( 0, 0, width, height );
	m_srcRect = rectBounds;

#ifdef PLAT_WINDOWS
	// First, set up the BITMAPINFO structure
	m_bitMapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_bitMapInfo.bmiHeader.biWidth = rectBounds.Width();
    m_bitMapInfo.bmiHeader.biHeight = rectBounds.Height();
    m_bitMapInfo.bmiHeader.biPlanes = 1;
	m_bitMapInfo.bmiHeader.biBitCount = bytesPerPixel * 8;
    m_bitMapInfo.bmiHeader.biCompression = BI_RGB;
    m_bitMapInfo.bmiHeader.biSizeImage = ((DWORD)rectBounds.Width()*8) * rectBounds.Height();
	m_bitMapInfo.bmiHeader.biXPelsPerMeter = 0;
    m_bitMapInfo.bmiHeader.biYPelsPerMeter = 0;
    m_bitMapInfo.bmiHeader.biClrUsed = 0;
    m_bitMapInfo.bmiHeader.biClrImportant = 0;

	CWindowDC wDC ( NULL );

	m_pBuffer = NULL;

	// Create a DIB Section
	m_hDIBSection = ::CreateDIBSection( wDC.GetSafeHdc(), &m_bitMapInfo,
		DIB_PAL_COLORS,
		(void**)&m_pBuffer, NULL, 0 );
	
	ASSERT ( m_hDIBSection );
	ASSERT ( m_pBuffer );

	if ( ( m_hDIBSection == NULL ) || ( m_pBuffer == NULL ) )
		AfxThrowMemoryException(); // what else could it be?

	m_bits.Attach ( m_hDIBSection );

	DIBSECTION dibSection;
	
	::GetObject( m_hDIBSection, sizeof ( dibSection ), &dibSection );
 
	int m_nRowBytes = WIDTHBYTES((DWORD)dibSection.dsBm.bmWidthBytes);
	int m_nPixelBitSize = dibSection.dsBm.bmBitsPixel;
	int m_nHeight = dibSection.dsBmih.biHeight;

	int bufferSize = abs ( m_nHeight ) * m_nRowBytes;
	memset ( m_pBuffer, 255, bufferSize );
#endif
}

#ifdef PLAT_WINDOWS
void COffscreenBuffer :: Draw ( CDC * pDC, const CRect & destRect )
{
	CDC newSrcDC;

	if ( newSrcDC.CreateCompatibleDC ( pDC ) )
	{
		CBitmap *pOldBits = newSrcDC.SelectObject ( &m_bits );
		if ( m_srcRect.Size() == destRect.Size() )
			pDC->BitBlt ( destRect.left, destRect.top, destRect.Width(), destRect.Height(),
				&newSrcDC, m_srcRect.left, m_srcRect.top, SRCCOPY );
		else
			pDC->StretchBlt ( destRect.left, destRect.top, destRect.Width(), destRect.Height(),
				&newSrcDC, m_srcRect.left, m_srcRect.top, m_srcRect.Width(), m_srcRect.Height(), SRCCOPY );
		newSrcDC.SelectObject ( pOldBits );
	}
}

void COffscreenBuffer :: Draw ( CDC * pDC, const CRect & rSrc, const CRect &destRect )
{
	CDC newSrcDC;

	if ( newSrcDC.CreateCompatibleDC ( pDC ) )
	{
		CBitmap *pOldBits = newSrcDC.SelectObject ( &m_bits );
		pDC->StretchBlt ( destRect.left, destRect.top, destRect.Width(), destRect.Height(),
			&newSrcDC, rSrc.left, rSrc.top, rSrc.Width(), rSrc.Height(), SRCCOPY );
		newSrcDC.SelectObject ( pOldBits );
	}
}

#endif
