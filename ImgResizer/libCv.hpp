#ifndef LIBCV_HPP_FD604ACF_B4B6_11E2_8357_005056C00008_INCLUDED_
#define LIBCV_HPP_FD604ACF_B4B6_11E2_8357_005056C00008_INCLUDED_

////////////////////////////////////////////////////////////////////////////////
static void cvxShowImage(CDC* pDC, const IplImage* iplimg_ptr);
static void cvxShowImage(Gdiplus::Graphics& graphics, IplImage* iplimg_ptr);
static void cvxShowImage(Gdiplus::Graphics& graphics, IplImage* iplimg_ptr, CRect& rect);
static void cvxShowImage(Gdiplus::Graphics& graphics, IplImage* iplimg_ptr, CvRect& rect);
static void cvxShowImage(Gdiplus::Graphics& graphics, IplImage* iplimg_ptr, Gdiplus::Rect& rect);
static IplImage* cvxRatioResize(IplImage* img_src, CvSize max_size);
static void cvxRoateImage(IplImage* iplimg_c1_original,
        IplImage* iplimg_c1_result,
        CvPoint2D32f center,
        int angle/*clockwish:0-359*/);
static CvPoint cvxGetDPI();

////////////////////////////////////////////////////////////////////////////////
static void cvxShowImage(CDC* pDC, const IplImage* iplimg_ptr)
{
    //Get DC dimension
    BITMAP bmp;
    memset(&bmp, 0, sizeof(BITMAP));

    HGDIOBJ hobj = GetCurrentObject(pDC->GetSafeHdc(), OBJ_BITMAP);
    GetObject(hobj, sizeof(BITMAP), &bmp);

    int dc_width  = bmp.bmWidth;
    int dc_height = bmp.bmHeight;

    //Build bmp info header
    const int bmi_len = sizeof(BITMAPINFOHEADER) + 256 * 4;

    BYTE buf[bmi_len];
    BITMAPINFO* bmi = (BITMAPINFO*)buf;
    memset(bmi, 0, bmi_len);

    //Fill header
    BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);
    bmih->biSize     = sizeof(BITMAPINFOHEADER);
    bmih->biWidth    = iplimg_ptr->width;
    bmih->biHeight   = iplimg_ptr->origin ? abs(iplimg_ptr->height) : -abs(iplimg_ptr->height);
    bmih->biPlanes   = 1;
    bmih->biBitCount = (iplimg_ptr->depth & 255) * iplimg_ptr->nChannels;
    bmih->biCompression = BI_RGB;

    //Fill look-up table
    if (bmih->biBitCount == 8) {
        RGBQUAD* palette = bmi->bmiColors;
        for (int i = 0;i < 256;i++) {
            palette[i].rgbBlue      = (BYTE)i;
            palette[i].rgbGreen     = (BYTE)i;
            palette[i].rgbRed       = (BYTE)i;
            palette[i].rgbReserved  = 0;
        }
    }

    int start_scan = 0;
    int lines = iplimg_ptr->height;

    SetDIBitsToDevice(pDC->GetSafeHdc(),
        //Destination
        0, 0, dc_width, dc_height,
        //Source
        0, 0,
        start_scan, lines,
        //Other
        iplimg_ptr->imageData, bmi, DIB_RGB_COLORS);
}

static void _cvxShowImage(Gdiplus::Graphics& graphics, IplImage* iplimg_ptr, Gdiplus::Rect& rect, bool bfull)
{
	if (iplimg_ptr->nChannels == 3) {
		//Convert to bitmap
		Gdiplus::Bitmap bitmap(iplimg_ptr->width, iplimg_ptr->height, iplimg_ptr->widthStep, 
			PixelFormat24bppRGB, (BYTE*)iplimg_ptr->imageData);
		//Draw
		if (bfull) {
			graphics.DrawImage(&bitmap, 0, 0);
		} else {
			graphics.DrawImage(&bitmap, rect);
		}
	} else if (iplimg_ptr->nChannels == 1) {
		//Convert to bitmap
		Gdiplus::Bitmap bitmap(iplimg_ptr->width, iplimg_ptr->height, iplimg_ptr->widthStep, 
			PixelFormat8bppIndexed, (BYTE*)iplimg_ptr->imageData);

		//Build index table
		Gdiplus::ColorPalette* color_table = (Gdiplus::ColorPalette*)malloc(
			sizeof(Gdiplus::ColorPalette) + 256 * sizeof(Gdiplus::ARGB));

		color_table->Count = 256;
		color_table->Flags = 0;

		for (int i = 0;i < 256;i++) {
			color_table->Entries[i] = Gdiplus::Color::MakeARGB(255, i, i, i);
		}

		//Copy to bitmap
		bitmap.SetPalette(color_table);
		free(color_table);

		//Draw
		if (bfull) {
			graphics.DrawImage(&bitmap, 0, 0);
		} else {
			graphics.DrawImage(&bitmap, rect);
		}
	}
}

static void cvxShowImage(Gdiplus::Graphics& graphics, IplImage* iplimg_ptr)
{
	Gdiplus::Rect rect;
	_cvxShowImage(graphics, iplimg_ptr, rect, true);
}

static void cvxShowImage(Gdiplus::Graphics& graphics, IplImage* iplimg_ptr, CRect& rect)
{
	_cvxShowImage(graphics, iplimg_ptr, 
		Gdiplus::Rect(rect.left, rect.top, rect.Width(), rect.Height()), false);
}

static void cvxShowImage(Gdiplus::Graphics& graphics, IplImage* iplimg_ptr, CvRect& rect)
{
	_cvxShowImage(graphics, iplimg_ptr, 
		Gdiplus::Rect(rect.x, rect.y, rect.width, rect.height), false);
}

static void cvxShowImage(Gdiplus::Graphics& graphics, IplImage* iplimg_ptr, Gdiplus::Rect& rect)
{
	_cvxShowImage(graphics, iplimg_ptr, rect, false);
}

static IplImage* cvxRatioResize(IplImage* img_src, CvSize max_size)
{
    CvSize cur_size = cvGetSize(img_src);
    bool bresize = false;

    float max_ratio = float(max_size.width) / float(max_size.height);
    float cur_ratio = float(cur_size.width) / float(cur_size.height);

    if (cur_ratio > max_ratio) {
        if (cur_size.width > max_size.width) {
            cur_size.width = max_size.width;
            cur_size.height = cvRound(cur_size.width / cur_ratio);
            bresize = true;
        }
    } else {
        if (cur_size.height > max_size.height) {
            cur_size.height = max_size.height;
            cur_size.width = cvRound(cur_size.height * cur_ratio);
            bresize = true;
        }
    }

    if (bresize) {
        IplImage* newimg = cvCreateImage(cur_size, img_src->depth, img_src->nChannels);
        cvResize(img_src, newimg, CV_BILATERAL);
        return newimg;
    } else {
        return cvCloneImage(img_src);
    }
}

static void cvxRoateImage(IplImage* iplimg_c1_original,
        IplImage* iplimg_c1_result,
        CvPoint2D32f center,
        int angle/*clockwish:0-359*/)
{
    float m[6];
    CvMat M = cvMat(2, 3, CV_32F, m);

    int w = iplimg_c1_original->width;
    int h = iplimg_c1_original->height;

    // Matrix m looks like:
    //
    // [ m0  m1  m2 ] ===>  [ A11  A12   b1 ]
    // [ m3  m4  m5 ]       [ A21  A22   b2 ]
    m[0] = (float)(cos(angle * (2 * CV_PI / 180.)));
    m[1] = (float)(sin(angle * (2 * CV_PI / 180.)));
    m[3] = -m[1];
    m[4] = m[0];
    //center
    m[2] = center.x;//x
    m[5] = center.y;//y

    cvZero(iplimg_c1_result);

    //dst(x,y) = A * src(x,y) + b
    cvGetQuadrangleSubPix(iplimg_c1_original, iplimg_c1_result, &M);
}

static CvPoint cvxGetDPI()
{
	//Number of pixels per logical inch along the screen width. Such like 96 DPI...
	//A 72-point font is rendered as 96 DPI or else...
	HDC screen = ::GetDC(0);
	int dpiX = GetDeviceCaps(screen, LOGPIXELSX);
	int dpiY = GetDeviceCaps(screen, LOGPIXELSY);
	ReleaseDC(0, screen);

	return cvPoint(dpiX, dpiY);
}

#endif

