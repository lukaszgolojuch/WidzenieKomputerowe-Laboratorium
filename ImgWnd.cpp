// ImgWnd.cpp : implementation file
//

#include "stdafx.h"
#include "PO1.h"
#include "ImgWnd.h"


// CImgWnd

IMPLEMENT_DYNAMIC(CImgWnd, CWnd)

CImgWnd::CImgWnd()
{
	m_pImg = NULL;
}

CImgWnd::~CImgWnd()
{
}


BEGIN_MESSAGE_MAP(CImgWnd, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CImgWnd message handlers

BOOL CImgWnd::Create(const RECT& rect, CWnd*  pParentWnd, UINT nID)
{
	BOOL bRes;

	bRes = CWnd::Create(NULL, NULL,
		WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, rect, pParentWnd, nID, NULL);

	ShowScrollBar(SB_BOTH, FALSE);

	return bRes;
}


void CImgWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// Do not call CWnd::OnPaint() for painting messages

	if (m_pImg)
	{
		CRect rDC;
		CRect rDIB(0, 0, m_pImg->GetWidth(), m_pImg->GetHeight());
		GetClientRect(rDC);

		double aspect;
		double aspect2 = ((double)(rDC.Width())) / ((double)(rDIB.Width()));
		double aspect3 = ((double)(rDC.Height())) / ((double)(rDIB.Height()));

		int iDestH, iDestW;
		int iOffX = 0;
		int iOffY = 0;

		double dHeight = (double)(rDIB.Height());
		if (dHeight*aspect2 > rDC.Height())
		{
			iDestH = rDC.Height();
			iDestW = (int)((double)(rDIB.Width())* aspect3);
			iOffX = (int)(rDC.Width() - iDestW) / 2;

			aspect = aspect3;
		}
		else
		{
			iDestH = (int)(dHeight*aspect2);
			iDestW = rDC.Width();
			iOffY = (int)(rDC.Height() - iDestH) / 2;

			aspect = aspect2;
		}

		Graphics gr(dc.GetSafeHdc());

		gr.DrawImage(m_pImg.get(), iOffX, iOffY, iDestW, iDestH);
	}
}
