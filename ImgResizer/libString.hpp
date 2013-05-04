#ifndef LIBSTRING_HPP_17130830_B49B_11E2_B58A_005056C00008_INCLUDED_
#define LIBSTRING_HPP_17130830_B49B_11E2_B58A_005056C00008_INCLUDED_

namespace string
{
    static char* unicode2mbcs(wchar_t* s)
    {
        //Calculate buffer size
        DWORD dw_num = WideCharToMultiByte(CP_OEMCP,
            NULL, s, -1, NULL, 0, NULL, FALSE);

        //Prepare buffer
        char* p = (char*)malloc(dw_num + 1);

        //Convert unicode to mbcs
        WideCharToMultiByte(CP_OEMCP,
            NULL, s, -1, p, dw_num, NULL, FALSE);

        return p;
    }

    static wchar_t* mbcs2unicode(char* s)
    {
        //Calculate buffer size
        DWORD dw_num = MultiByteToWideChar(CP_OEMCP,
            NULL, s, -1, NULL, 0);

        //Prepare buffer
        wchar_t* p = (wchar_t*)malloc((dw_num + 1) * sizeof(wchar_t));

        //Convert unicode to mbcs
        MultiByteToWideChar(CP_OEMCP,
            NULL, s, -1, p, dw_num);

        return p;
    }
}//namespace string

#endif

