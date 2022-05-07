#pragma once
#include <GdiPlus.h>
#include <memory>
using namespace Gdiplus;

// CImgWnd

class CImgWnd : public CWnd
{
	DECLARE_DYNAMIC(CImgWnd)

public:
	CImgWnd();
	virtual ~CImgWnd();

	BOOL Create(const RECT& rect, CWnd*  pParentWnd, UINT nID);
	void SetImg(std::shared_ptr<Bitmap> pImg){ m_pImg = pImg; };

protected:
	std::shared_ptr<Bitmap> m_pImg;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


