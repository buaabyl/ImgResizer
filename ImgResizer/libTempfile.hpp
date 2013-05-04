#ifndef LIBTEMPFILE_HPP_69C481C0_B49C_11E2_9B0E_005056C00008_INCLUDED_
#define LIBTEMPFILE_HPP_69C481C0_B49C_11E2_9B0E_005056C00008_INCLUDED_

namespace tempfile
{
    static CString mktemp(CString prefix = _T("tmp"))
    {
        TCHAR temp_path[MAX_PATH];
        TCHAR temp_name[MAX_PATH];

        if (GetTempPath(MAX_PATH, temp_path) > 0) {
            if (GetTempFileName(temp_path, prefix, 3, temp_name) > 0 ) {
                return CString(temp_name);
            }
        }

        return CString();
    }
}//namespace tempfile

#endif

