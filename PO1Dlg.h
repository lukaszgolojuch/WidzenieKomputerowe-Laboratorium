
// PO1Dlg.h : header file
//

#pragma once
#include "ImgWnd.h"
#include "afxwin.h"
#include <GdiPlus.h>
#include <memory>
using namespace Gdiplus;


// CPODlg dialog
class CPODlg : public CDialogEx
{
// Construction
public:
	CPODlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PO1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	CImgWnd m_imgWndIN;
	CImgWnd m_imgWndOUT;
	std::shared_ptr<Bitmap> m_pImgIN;
	std::shared_ptr<Bitmap> m_pImgOUT;
	int m_imgW;
	int m_imgH;
	CString params;

	CComboBox m_combo1;

	GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR           m_gdiplusToken;

	BYTE GetPixel(int x, int y);
	void SetPixel(int x, int y, BYTE J);
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonProcess();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonParams();
	afx_msg void OnDestroy();
	
};
