#ifndef LIBCMEMORYDC_HPP_F25A7ACF_B49C_11E2_A813_005056C00008_INCLUDED_
#define LIBCMEMORYDC_HPP_F25A7ACF_B49C_11E2_A813_005056C00008_INCLUDED_

namespace CMemoryDC
{
    class CMemoryDC
    {
    public:
        CMemoryDC(CWnd* wnd)
        {
            wnd_ = wnd;
            if (wnd == NULL) {
                return;
            }

            //Get size
            wnd->GetClientRect(&rect_);
            int width = rect_.Width();
            int height= rect_.Height();

            //Obtain graphic device resource
            cdc_ = wnd->GetDC();

            //Create frame buffer
            memory_dc_.CreateCompatibleDC(NULL);
            memory_bitmap_.CreateCompatibleBitmap(cdc_, width, height);
            memory_dc_.SelectObject(&memory_bitmap_);
        }

        ~CMemoryDC()
        {
            if (wnd_ == NULL) {
                return;
            }

            //Cache size
            int width = rect_.Width();
            int height= rect_.Height();

            //Copy to surface
            cdc_->BitBlt(0, 0, width, height, &memory_dc_, 0, 0, SRCCOPY);

            //Release resources
            memory_bitmap_.DeleteObject();
            memory_dc_.DeleteDC();
            wnd_->ReleaseDC(cdc_);
        }

    public:
        CDC* GetDC()
        {
            return &memory_dc_;
        }

        int ReleaseDC(CDC* pDC)
        {
            return 0;
        }

    private:
        CWnd* wnd_;
        CDC* cdc_;
        CRect rect_;

    private:
        CDC memory_dc_;
        CBitmap memory_bitmap_;
    };
}//namespace CMemoryDC

#endif

