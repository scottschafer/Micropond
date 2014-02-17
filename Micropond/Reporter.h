// Reporter.h: interface for the Reporter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORTER_H__161834A0_3200_494E_83AF_97EC06A8928A__INCLUDED_)
#define AFX_REPORTER_H__161834A0_3200_494E_83AF_97EC06A8928A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class Reporter  
{
public:
	Reporter();
	virtual ~Reporter();

	void	NewTime();
	void	AddSignature ( LPCSTR pSig, int nCount = 1 );
	void	WriteReport ( void );

private:
	CMap<CString,LPCSTR,CMap<int,int,int,int>*,CMap<int,int,int,int>*> m_mapHistory;
	int		m_time;
};

extern Reporter gReporter;

#endif // !defined(AFX_REPORTER_H__161834A0_3200_494E_83AF_97EC06A8928A__INCLUDED_)
