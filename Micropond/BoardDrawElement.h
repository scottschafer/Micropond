
class BoardDrawElement
{
public:
	BoardDrawElement ( LPCSTR pName );

	void	Draw ( int iOrientation, int iSize, CPoint pt, LPWORD *pBuffer, int rowPixelsBuffer );

	WORD *	m_bits[2][4];
};

