
// PO1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "PO1.h"
#include "PO1Dlg.h"
#include "ParamsDlg.h"
#include "afxdialogex.h"
#include <algorithm>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IMG_WND_ID_IN	100
#define IMG_WND_ID_OUT	101

constexpr bool INVERT_COLORS = true; // wszystko robią na tej uczelni odwrotnie... Polsko-Australijska normalnie.

constexpr auto BINARY_THRESHOLD = 128;
constexpr auto OPTION_DILATE_BINARY_3x3 = L"binary dilation 3x3";
constexpr auto OPTION_DILATE_BINARY_5x5 = L"binary dilation 5x5";
constexpr auto OPTION_DILATE_BINARY_9x9 = L"binary dilation 9x9";
constexpr auto OPTION_DILATE_3x3 = L"dilation 3x3";
constexpr auto OPTION_DILATE_5x5 = L"dilation 5x5";
constexpr auto OPTION_ERODE = L"erosion";
constexpr auto OPTION_OPEN = L"opening";
constexpr auto OPTION_CLOSE = L"closing";
constexpr auto OPTION_INNER_BOUNDARY = L"inner boundary";
constexpr auto OPTION_OUTER_BOUNDARY = L"outer boundary";
std::vector<const wchar_t*> OPTIONS = { OPTION_DILATE_BINARY_3x3, OPTION_DILATE_BINARY_5x5, OPTION_DILATE_BINARY_9x9, OPTION_DILATE_3x3, OPTION_DILATE_5x5, OPTION_ERODE, OPTION_OPEN, OPTION_CLOSE, OPTION_INNER_BOUNDARY, OPTION_OUTER_BOUNDARY };

template<typename T>
T mod(T v, T m)
{
	return (v % m + m) % m;
}

struct Vec2
{
	int x, y;
	friend static Vec2 operator+(const Vec2& lhs, const Vec2& rhs)
	{
		return { lhs.x + rhs.x, lhs.y + rhs.y };
	}
	friend static Vec2 operator-(const Vec2& lhs, const Vec2& rhs)
	{
		return { lhs.x - rhs.x, lhs.y - rhs.y };
	}
	friend static Vec2 operator*(const Vec2& lhs, const Vec2& rhs)
	{
		return { lhs.x * rhs.x, lhs.y * rhs.y };
	}
	friend static Vec2 operator/(const Vec2& lhs, const Vec2& rhs)
	{
		return { lhs.x / rhs.x, lhs.y / rhs.y };
	}
	friend static Vec2 operator%(const Vec2& lhs, const Vec2& rhs)
	{
		return { mod(lhs.x, rhs.x), mod(lhs.y, rhs.y) };
	}
	friend static Vec2 operator+(const Vec2& lhs, int rhs)
	{
		return { lhs.x + rhs, lhs.y + rhs };
	}
	friend static Vec2 operator-(const Vec2& lhs, int rhs)
	{
		return { lhs.x - rhs, lhs.y - rhs };
	}
	friend static Vec2 operator*(const Vec2& lhs, int rhs)
	{
		return { lhs.x * rhs, lhs.y * rhs };
	}
	friend static Vec2 operator/(const Vec2& lhs, int rhs)
	{
		return { lhs.x / rhs, lhs.y / rhs };
	}
	friend static Vec2 operator%(const Vec2& lhs, int rhs)
	{
		return { mod(lhs.x, rhs), mod(lhs.y, rhs) };
	}
	friend static bool operator==(const Vec2& lhs, const Vec2& rhs)
	{
		return (lhs.x == rhs.x && lhs.y == rhs.y);
	}
	friend static bool operator!=(const Vec2& lhs, const Vec2& rhs)
	{
		return (lhs.x != rhs.x || lhs.y != rhs.y);
	}
	Vec2()
	{
		x = 0;
		y = 0;
	}
	Vec2(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	Vec2(const Vec2& other)
	{
		this->x = other.x;
		this->y = other.y;
	}
};

class SensibleImage
{
protected:
	shared_ptr<Bitmap> img;
public:
	int width;
	int height;
	Vec2 size;
	int pixels;

	shared_ptr<Bitmap> GetImageShared()
	{
		return img;
	}

	Vec2 CoordinatesFromIndex(int xy)
	{
		xy = mod(xy, pixels);
		return { xy % width, xy / width };
	}

	int GetPixel(Vec2 pos)
	{
		pos = pos % size;
		Color pixelColor;
		img->GetPixel(pos.x, pos.y, &pixelColor);

		double r = pixelColor.GetR();
		double g = pixelColor.GetG();
		double b = pixelColor.GetB();
		double J = 0.299 * r + 0.587 * g + 0.114 * b;
		uint8_t ret = J;
		if constexpr (INVERT_COLORS)
			ret = 255 - J;
		return ret;
	}

	int GetPixel(int x, int y)
	{
		return GetPixel({ x, y });
	}
	int GetPixel(int xy)
	{
		return GetPixel(CoordinatesFromIndex(xy));
	}

	uint8_t SetPixel(Vec2 pos, int v)
	{
		pos = pos % size;
		v = (v < 0) ? 0 : (v > 255) ? 255 : v;

		if (INVERT_COLORS)
			v = 255 - v;

		img->SetPixel(pos.x ,pos.y, Color(v, v, v));
		return v;
	}

	uint8_t SetPixel(int x, int y, int v)
	{
		return SetPixel({ x, y }, v);
	}
	uint8_t SetPixel(int xy, int v)
	{
		return SetPixel(CoordinatesFromIndex(xy), v);
	}
	SensibleImage(Vec2 size)
	{
		img = make_shared<Bitmap>(size.x, size.y, PixelFormat32bppARGB);
		this->size = size;
		width = size.x;
		height = size.y;
		pixels = width * height;
	}
	SensibleImage(Bitmap* nimg, bool copy)
	{
		width = nimg->GetWidth();
		height = nimg->GetHeight();
		size = { width, height };
		pixels = width * height;
		if (copy)
			img = shared_ptr<Bitmap>(nimg->Clone(0, 0, width, height, PixelFormat32bppARGB));
		else
			img = shared_ptr<Bitmap>(nimg);
	}
	SensibleImage(shared_ptr<Bitmap> nimg, bool copy)
	{
		width = nimg->GetWidth();
		height = nimg->GetHeight();
		size = { width, height };
		pixels = width * height;
		if (copy)
			img = shared_ptr<Bitmap>(nimg->Clone(0, 0, width, height, PixelFormat32bppARGB));
		else
			img = nimg;
	}
	SensibleImage(const SensibleImage& other) = delete;
};

struct Mask
{
	Vec2 size;
	Vec2 offset;
	std::vector<Vec2> GetOffsets()
	{
		vector<Vec2> ret;
		ret.reserve(size.x * size.y);
		for (int x = 0; x < size.x; x++)
		{
			for (int y = 0; y < size.y; y++)
			{
				Vec2 p = Vec2{ x, y } - offset;
				if (p == Vec2{ 0,0 })
					continue;
				ret.push_back(p);
			}
		}
		return ret;
	}
};
using MorphFunc = std::function<int(int centre, const vector<int>& neighbors)>;
shared_ptr<SensibleImage> MorphImage(shared_ptr<SensibleImage> input, Mask mask, MorphFunc f)
{
	shared_ptr<SensibleImage> output = make_shared<SensibleImage>(input->size);
	vector<Vec2> offsets = mask.GetOffsets();
	vector<int> neighbours;
	neighbours.resize(offsets.size());
	for (int x = 0; x < input->width; x++)
	{
		for (int y = 0; y < input->height; y++)
		{
			Vec2 p = { x, y };
			for (int i = 0; i < offsets.size(); i++)
			{
				neighbours[i] = input->GetPixel(p + offsets[i]);
			}
			output->SetPixel(p, f(input->GetPixel(p), neighbours));
		}
	}

	return output;
}
using CombineFunc = std::function<int(int p0, int p1)>;
shared_ptr<SensibleImage> CombineImages(shared_ptr<SensibleImage> input0, shared_ptr<SensibleImage> input1, CombineFunc f)
{
	ASSERT(input0->size == input1->size);
	int pixels = input0->pixels;
	shared_ptr<SensibleImage> output = make_shared<SensibleImage>(input0->size);
	for (int i = 0; i < pixels; i++)
	{
		output->SetPixel(i, f(input0->GetPixel(i), input1->GetPixel(i)));
	}
	return output;
}

shared_ptr<SensibleImage> DilateBinary(shared_ptr<SensibleImage> input, Mask mask)
{
	return MorphImage(input, mask, (MorphFunc)[](int centre, const std::vector<int>& n)
		{
			int maximumelement = *max_element(n.begin(), n.end());
			if (maximumelement < (int)centre)
			{
				maximumelement = (int)centre;
			}

			if (maximumelement > BINARY_THRESHOLD)
			{
				return 255;
			}
			else {
				return 0;
			}
		});
}
shared_ptr<SensibleImage> Dilate(shared_ptr<SensibleImage> input, Mask mask)
{
	return MorphImage(input, mask, (MorphFunc)[](int centre, const std::vector<int>& n)
		{
			int maximumelement = *max_element(
				n.begin(),
				n.end());

			if (maximumelement < (int)centre)
			{
				maximumelement = (int)centre;
			}
			return maximumelement;
		});
}
shared_ptr<SensibleImage> Erode(shared_ptr<SensibleImage> input, Mask mask)
{
	return MorphImage(input, mask, (MorphFunc)[](int centre, const std::vector<int>& n)
		{
			int minimumelement = *min_element(n.begin(), n.end());
			if (minimumelement > centre)
			{
				minimumelement = centre;
			}
			return minimumelement;
		});
}
shared_ptr<SensibleImage> Open(shared_ptr<SensibleImage> input, Mask mask)
{
	shared_ptr<SensibleImage> afterErosion = Erode(input, mask);
	shared_ptr<SensibleImage> afterDilatation = Dilate(afterErosion, mask);
	return afterDilatation;
}
shared_ptr<SensibleImage> Close(shared_ptr<SensibleImage> input, Mask mask)
{
	shared_ptr<SensibleImage> afterDilatation = Dilate(input, mask);
	return Erode(afterDilatation, mask);
}
shared_ptr<SensibleImage> InnerBoundary(shared_ptr<SensibleImage> input, Mask mask)
{
	return CombineImages(input, Erode(input, mask), [](int p0, int p1)
		{
			return p0 - p1;
		});
}
shared_ptr<SensibleImage> OuterBoundary(shared_ptr<SensibleImage> input, Mask mask)
{
	return CombineImages(Dilate(input, mask), input, [](int p0, int p1)
		{
			return p0 - p1;
		});
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPODlg dialog



CPODlg::CPODlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPODlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pImgIN = m_pImgOUT = NULL;
	m_imgH = m_imgW = -1;
}

void CPODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combo1);
}

BEGIN_MESSAGE_MAP(CPODlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CPODlg::OnBnClickedButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CPODlg::OnBnClickedButtonProcess)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CPODlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_PARAMS, &CPODlg::OnBnClickedButtonParams)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CPODlg message handlers

BOOL CPODlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);


	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	CRect rDlg(7,7,407,407);
	MapDialogRect(rDlg);

	m_imgWndIN.Create(rDlg, this, IMG_WND_ID_IN);

	rDlg = CRect(530, 7, 930, 407);
	MapDialogRect(rDlg);

	m_imgWndOUT.Create(rDlg, this, IMG_WND_ID_OUT);

	for(auto it : OPTIONS)
	{
		m_combo1.AddString(it);
	}
	m_combo1.SelectString(0, OPTIONS[0]);



	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPODlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPODlg::OnBnClickedButtonLoad()
{
	WCHAR strFilter[] = { L"Image Files (*.bmp; *.jpg; *.tiff; *.gif; *.png)|*.bmp; *.jpg; *.tiff; *.gif; *.png|All Files (*.*)|*.*||" };

	CFileDialog FileDlg(TRUE, NULL, NULL, 0, strFilter);

	if (FileDlg.DoModal() == IDOK)
	{

		m_pImgIN = shared_ptr<Bitmap>(Bitmap::FromFile(FileDlg.GetPathName()));
		m_imgWndIN.SetImg(m_pImgIN);
		m_imgW = m_pImgIN->GetWidth();
		m_imgH = m_pImgIN->GetHeight();

		m_pImgOUT = make_shared<Bitmap>(m_imgW, m_imgH, PixelFormat32bppARGB);// PixelFormat16bppGrayScale);

		m_imgWndOUT.SetImg(m_pImgOUT);

		Invalidate();
	}
}


void CPODlg::OnBnClickedButtonProcess()
{
	CString sOption;
	m_combo1.GetLBText(m_combo1.GetCurSel(), sOption);
	BeginWaitCursor();

	shared_ptr<SensibleImage> input = make_shared<SensibleImage>(m_pImgIN, false);
	shared_ptr<SensibleImage> output;

	auto mask3x3 = Mask{ {3, 3}, {1, 1} };
	auto mask5x5 = Mask{ {5, 5}, {2, 2} };
	auto mask9x9 = Mask{ {9, 9}, {5, 5} };
	if (sOption == OPTION_DILATE_BINARY_3x3)
	{
		output = DilateBinary(input, mask3x3);
	}
	else if (sOption == OPTION_DILATE_BINARY_5x5)
	{
		output = DilateBinary(input, mask5x5);
	}
	else if (sOption == OPTION_DILATE_BINARY_9x9)
	{
		output = DilateBinary(input, mask9x9);
	}
	else if (sOption == OPTION_DILATE_3x3)
	{
		output = Dilate(input, mask3x3);
	}
	else if (sOption == OPTION_DILATE_5x5)
	{
		output = Dilate(input, mask5x5);
	}
	else if (sOption == OPTION_ERODE)
	{
		output = Erode(input, mask3x3);
	}
	else if (sOption == OPTION_OPEN)
	{
		output = Open(input, mask3x3);
	}
	else if (sOption == OPTION_CLOSE)
	{
		output = Close(input, mask3x3);
	}
	else if (sOption == OPTION_INNER_BOUNDARY)
	{
		output = InnerBoundary(input, mask3x3);
	}
	else if (sOption == OPTION_OUTER_BOUNDARY)
	{
		output = OuterBoundary(input, mask3x3);
	}

	if (output != nullptr)
	{
		m_pImgOUT = output->GetImageShared();
		m_imgWndOUT.SetImg(m_pImgOUT);
	}

	Invalidate();
	EndWaitCursor();
}


void CPODlg::OnBnClickedButtonSave()
{
	WCHAR strFilter[] = { L"Bitmap (*.bmp)|*.bmp|JPEG (*.jpg)|*.jpg||" };

	CFileDialog FileDlg(FALSE, NULL, NULL, 0, strFilter);

	if (FileDlg.DoModal() == IDOK)
	{
		CLSID Clsid;
		CString sClsId;
		CString sPath = FileDlg.GetPathName();
		CString sExt = (FileDlg.GetFileExt()).MakeUpper();

		if (sExt == "")
		{
			switch (FileDlg.m_ofn.nFilterIndex)
			{
			case 2:
				sExt = "JPG";
				break;
			default:
				sExt = "BMP";
			}

			sPath += L"." + sExt;
		}

		if (sExt == "BMP")
			sClsId = "image/bmp";

		if (sExt == "JPG")
			sClsId = "image/jpeg";

		GetEncoderClsid(sClsId, &Clsid);
		m_pImgOUT->Save(sPath, &Clsid, NULL);
	}
}


void CPODlg::OnBnClickedButtonParams()
{
	CParamsDlg paramsDlg;
	CString s;

	if (paramsDlg.DoModal() == IDOK)
	{
		s = paramsDlg.m_sParams;
		params = s;
	}
}


void CPODlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	GdiplusShutdown(m_gdiplusToken);

	// TODO: Add your message handler code here
}


BYTE CPODlg::GetPixel(int x, int y)
{
	Color pixelColor;
	m_pImgIN->GetPixel(x, y, &pixelColor);

	double r = pixelColor.GetR();
	double g = pixelColor.GetG();
	double b = pixelColor.GetB();
	double J = 0.299*r + 0.587*g + 0.114*b;

	return (BYTE)J;
}

void CPODlg::SetPixel(int x, int y, BYTE J)
{
	Color pixelColor(J,J,J);
	Status s = m_pImgOUT->SetPixel(x, y, pixelColor);
}

int CPODlg::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}