
// ImgResizerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImgResizer.h"
#include "ImgResizerDlg.h"
#include "afxdialogex.h"

#include "libLoadImage.hpp"
#include "libPlatform.hpp"
#include "libOs.hpp"
#include "libString.hpp"
#include "libTempfile.hpp"
#include "libCMemoryDC.hpp"
#include "libCv.hpp"

static std::list<CString> _g_images_list;
static CString _g_outdir;

static bool resize(CString source_image, CString destination_outdir, CvSize max_size)
{
    CString system_temporary_filename = tempfile::mktemp();
    if (system_temporary_filename.GetLength(), 0) {
        return false;
    }

    //split filepath,filename,extension
    std::pair<CString, CString> res;

    res = os::path::split(source_image);
    CString file_path = res.first;
    CString file_name = res.second;

    res = os::path::splitext(file_name);
    CString file_name_no_ext = res.first;
    CString file_ext = res.second;

    //output image path, if have the same name rename it:)
    CString destination_image = os::path::join(destination_outdir, file_name);
    if (os::path::isfile(destination_image)) {
        CString new_name;
        new_name.Format(_T(".%x"), time(NULL));
        new_name = file_name_no_ext + new_name + file_ext;
        destination_image = os::path::join(destination_outdir, new_name);
    }

    //temporary image name
    CString temporary_filename;
    temporary_filename = system_temporary_filename + file_ext;

    //copy
    CopyFile(source_image, temporary_filename, FALSE);

    //convert
    char* ascii_tmp_name = string::unicode2mbcs(temporary_filename.GetBuffer());

    IplImage* img_src = cvLoadImage(ascii_tmp_name, CV_LOAD_IMAGE_UNCHANGED);
    DeleteFile(temporary_filename);

    if (img_src == NULL) {
        free(ascii_tmp_name);
        return false;
    }

    IplImage* img_dst = cvxRatioResize(img_src, max_size);
    cvSaveImage(ascii_tmp_name, img_dst);

    //clean
    free(ascii_tmp_name);
    cvReleaseImage(&img_src);
    cvReleaseImage(&img_dst);

    //copy
    CopyFile(temporary_filename, destination_image, FALSE);
    DeleteFile(temporary_filename);

    return true;
}

static UINT worker(CImgResizerDlg* dlg)
{
    CvSize max_size = dlg->size_resolution_[dlg->idx_resolution_];
    int index = 0;
    int nr_total = _g_images_list.size();

    for (auto p = _g_images_list.begin();p != _g_images_list.end();p++) {
        resize(*p, _g_outdir, max_size);
        dlg->PostMessage(WM_UPDATE_PROGRESS, ++index, nr_total);
    }

    return 0;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

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


// CImgResizerDlg dialog


CImgResizerDlg::CImgResizerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImgResizerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    size_resolution_[0].width = 640;
    size_resolution_[0].height= 480;

    size_resolution_[1].width = 800;
    size_resolution_[1].height= 600;

    size_resolution_[2].width = 1024;
    size_resolution_[2].height= 768;

    size_resolution_[3].width = 1280;
    size_resolution_[3].height= 1024;

    size_resolution_[4].width = 1680;
    size_resolution_[4].height= 1050;

    size_resolution_[5].width = 1920;
    size_resolution_[5].height= 1200;

    brunning_ = false;
}

void CImgResizerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_PICBOX, m_picbox_show_);
    DDX_Control(pDX, IDC_PROGRESS_PERCENT, m_progressctrl_percent_);
    DDX_Control(pDX, IDC_RADIO_640X480, m_radio_resolution_[0]);
    DDX_Control(pDX, IDC_RADIO_800X600, m_radio_resolution_[1]);
    DDX_Control(pDX, IDC_RADIO_1024X768, m_radio_resolution_[2]);
    DDX_Control(pDX, IDC_RADIO_1280X1024, m_radio_resolution_[3]);
    DDX_Control(pDX, IDC_RADIO_1680X1050, m_radio_resolution_[4]);
    DDX_Control(pDX, IDC_RADIO_1920X1200, m_radio_resolution_[5]);
    DDX_Control(pDX, IDC_STATIC_GROUP, m_static_group_);
}

BEGIN_MESSAGE_MAP(CImgResizerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_WM_DROPFILES()
    ON_MESSAGE(WM_UPDATE_PROGRESS, &CImgResizerDlg::OnUpdateProgress)
END_MESSAGE_MAP()


// CImgResizerDlg message handlers

BOOL CImgResizerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
    DWORD ver_digit = platform::GET_WIN32_WINNT();
    if (ver_digit >= _WIN32_WINNT_VISTA) {
        ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ALLOW);
    }

    m_radio_resolution_[0].SetCheck(BST_CHECKED);
    idx_resolution_ = 0;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CImgResizerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CImgResizerDlg::OnPaint()
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

    //Custom draw
    CustomPaint();

}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CImgResizerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CImgResizerDlg::OnDropFiles(HDROP hDropInfo)
{
    //Disable when converting
    if (brunning_) {
        return;
    }

    //Get size
    for (int i = 0;i < 6;i++) {
        if (m_radio_resolution_[i].GetCheck() == BST_CHECKED) {
            idx_resolution_ = i;
            break;
        }
    }

    //Fill image list
    TCHAR str_filename[MAX_PATH + 1] = {0};
    int nr_files = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

    CString msg;
    for (int i = 0;i < nr_files;i++) {
        DragQueryFile(hDropInfo, i, str_filename, MAX_PATH);
        _g_images_list.push_back(str_filename);
    }
    DragFinish(hDropInfo);

    //Generate outdir
    std::pair<CString, CString> res;
    res = os::path::split(CString(str_filename));

    CString dir_name;
    dir_name.Format(_T("resize-%08x"), time(NULL));
    CString outdir = os::path::join(res.first, dir_name);

    if (!os::path::isdir(outdir)) {
        CreateDirectory(outdir, NULL);
    }

    _g_outdir = outdir;

    //Convert
    m_progressctrl_percent_.SetRange32(0, _g_images_list.size());
    m_progressctrl_percent_.SetPos(0);

    //Change state
    this->GetWindowText(title_);
    CString new_title = title_ + _T(" (Running)");
    this->SetWindowText(new_title);
    this->brunning_ = true;

    for (int i = 0;i < 6;i++) {
        m_radio_resolution_[i].EnableWindow(FALSE);
    }

    PostMessage(WM_PAINT);

    AfxBeginThread((AFX_THREADPROC)worker, this);
}

LRESULT CImgResizerDlg::OnUpdateProgress(WPARAM nr_converted, LPARAM nr_total)
{
    int max_pos;
    int min_pos;
    m_progressctrl_percent_.GetRange(min_pos, max_pos);

    //Update percent
    percent_ = cvRound(100.0 * nr_converted / nr_total);
    if (percent_ < 0) {
        percent_ = 0;
    }
    if (percent_ > 100) {
        percent_ = 100;
    }

    //Check
    if (nr_converted == nr_total) {
        //Reset state
        m_progressctrl_percent_.SetPos(max_pos);
        this->SetWindowText(title_);
        this->brunning_ = false;

        //Indicate
        AfxMessageBox(_T("All done:)"));

        //Reset
        for (int i = 0;i < 6;i++) {
            m_radio_resolution_[i].EnableWindow(TRUE);
        }

        _g_images_list.clear();
        m_progressctrl_percent_.SetPos(0);
    } else {
        m_progressctrl_percent_.SetPos(nr_converted);
    }

    PostMessage(WM_PAINT);
    return 0;
}

void CImgResizerDlg::CustomPaint()
{
    CRect rect;
    m_picbox_show_.GetClientRect(&rect);
    int width = rect.Width();
    int height = rect.Height();
    int x = rect.left;
    int y = rect.top;

    CMemoryDC::CMemoryDC dc(&m_picbox_show_);
    CDC* pDC = dc.GetDC();

    CBrush brush_white(RGB(200,200,200));
    CBrush brush_grey(RGB(128,128,128));
    CBrush* p_old_brush;

    //Background
    pDC->FillRect(&rect, &brush_white);

    //left-bottom small box
    p_old_brush = pDC->SelectObject(&brush_grey);
    CRect rect_box;
    rect_box.right  = rect.left + 3*width/4;
    rect_box.top    = rect.top + 1*height/4;
    rect_box.left   = rect.left + 1;
    rect_box.bottom = rect.bottom - 1;
    pDC->FillRect(&rect_box, &brush_grey);
    pDC->SelectObject(p_old_brush);

    //line
    POINT pt1;
    POINT pt2;
    CPen pen_grey1(PS_SOLID, 3, RGB(150,150,150));
    CPen pen_grey2(PS_SOLID, 2, RGB(128,128,128));
    CPen pen(PS_SOLID, 1, RGB(0,0,0));
    CPen* p_old_pen;

    pt1.x = rect.left + width;
    pt1.y = rect.top - 1;
    pt2.x = rect.left + 3*width/4;
    pt2.y = rect.top + 1*height/4;

    POINT pt3 = pt2;
    POINT pt4 = pt2;
    pt3.y -= 40;
    pt4.x += 40;

    p_old_pen = pDC->SelectObject(&pen_grey1);
    pDC->MoveTo(pt1);
    pDC->LineTo(pt2);
    pDC->MoveTo(pt2);
    pDC->LineTo(pt3);
    pDC->MoveTo(pt2);
    pDC->LineTo(pt4);
    pDC->SelectObject(p_old_pen);

    p_old_pen = pDC->SelectObject(&pen_grey2);
    pDC->MoveTo(pt1);
    pDC->LineTo(pt2);
    pDC->MoveTo(pt2);
    pDC->LineTo(pt3);
    pDC->MoveTo(pt2);
    pDC->LineTo(pt4);
    pDC->SelectObject(p_old_pen);

    p_old_pen = pDC->SelectObject(&pen);
    pDC->MoveTo(pt1);
    pDC->LineTo(pt2);
    pDC->MoveTo(pt2);
    pDC->LineTo(pt3);
    pDC->MoveTo(pt2);
    pDC->LineTo(pt4);
    pDC->SelectObject(p_old_pen);

    //Text
    CString str_info;
    if (this->brunning_) {
        str_info.Format(_T("%d%%"), percent_);
    } else {
        str_info = _T("Drop files here");
    }
    CRect rect_type(
        x/*left*/,
        y/*top*/,
        x + width/*right*/,
        y + height/*bottom*/);
    pDC->SetBkMode(TRANSPARENT);
    pDC->DrawText(str_info, &rect_type, DT_CENTER|DT_CALCRECT);
    int type_width   = rect_type.Width();
    int type_height  = rect_type.Height();
    rect_type.top    = y + (height - type_height) / 2;
    rect_type.bottom = rect_type.top + type_height;
    rect_type.left   = x + (width - type_width) / 2;
    rect_type.right  = rect_type.left + type_width;

    COLORREF old_color = pDC->SetTextColor(RGB(255,255,255));
    pDC->DrawText(str_info, &rect_type, DT_CENTER);
    pDC->SetTextColor(old_color);

    dc.ReleaseDC(pDC);
}
