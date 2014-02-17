#pragma once


// CInstructionSetDlg dialog

class CInstructionSetDlg : public CDialog
{
	DECLARE_DYNAMIC(CInstructionSetDlg)

public:
	CInstructionSetDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInstructionSetDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	int m_bInstructionM;
	int m_bInstructionZ;
	int m_bInstructionA;
	int m_bInstructionRotLeft;
	int m_bInstructionRotRight;
	int m_bInstructionU;
	int m_bInstructionD;
	int m_bInstructionR;
	int m_bInstructionL;
	int m_bInstruction0;
	int m_bInstruction1;
	int m_bInstruction2;
	int m_bInstruction3;
	int m_bInstruction4;
	int m_bInstruction5;
	int m_bInstruction6;
	int m_bInstruction7;
	int m_bInstruction8;
	int m_bInstruction9;
	int m_bInstructionE;
	int m_bInstructione;
	int m_bInstructionW;
	int m_bInstructionH;
	int m_bInstructionMarkHead;
	int m_bInstructionRestart;
};
