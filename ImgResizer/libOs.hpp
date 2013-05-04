#ifndef LIBOS_HPP_7DEBA4C0_B498_11E2_8AA8_005056C00008_INCLUDED_
#define LIBOS_HPP_7DEBA4C0_B498_11E2_8AA8_005056C00008_INCLUDED_

#ifndef _T
#ifdef UNICODE
#define _T(s) L##s
#define _tcscmp         wcscmp
#else
#define _T(s) s
#define _tcscmp         strcmp
#endif
#endif

inline const char* c_str(std::string& s)
{
    return s.c_str();
}

inline const wchar_t* c_str(std::wstring& s)
{
    return s.c_str();
}

namespace os
{
    namespace path
    {
        static BOOL isfile(CString outdir)
        {
            DWORD dw_attrib = GetFileAttributes(outdir);
            if (dw_attrib == INVALID_FILE_ATTRIBUTES) {
                return FALSE;
            }
                
            if ((dw_attrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
                return FALSE;
            }

            return TRUE;
        }

        static BOOL isdir(CString outdir)
        {
            DWORD dw_attrib = GetFileAttributes(outdir);
            if (dw_attrib == INVALID_FILE_ATTRIBUTES) {
                return FALSE;
            }
                
            if ((dw_attrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
                return TRUE;
            }

            return FALSE;
        }

        static CString join(CString file_path, CString file_name)
        {
            int len = file_path.GetLength();
            if (file_path.GetAt(len - 1) != _T('\\')) {
                file_path += _T('\\');
            }

            return file_path + file_name;
        }

        //return path (without '\\') and file name
        static std::pair<CString, CString> split(CString& s)
        {
            //Split file name with ext
            auto lpstr = s.GetBuffer();
            int n = s.GetLength();
            lpstr += n -1;

            CString file_path = s;
            CString file_name_with_ext;

            for (int i = n - 1;i >= 0;i--) {
                if (*lpstr == _T('\\')) {
                    file_path.Delete(i, n - i);
                    file_name_with_ext = lpstr + 1;
                    break;
                }
                lpstr--;
            }

            if (*lpstr != _T('\\')) {
                return std::pair<CString, CString>();
            }

            return std::pair<CString, CString>(file_path, file_name_with_ext);
        }

        //return file name (without ".jpg") and extension
        static std::pair<CString, CString> splitext(CString& s)
        {
            auto lpstr = s.GetBuffer();
            int len = s.GetLength();

            CString file_name = s;
            CString file_ext;
            for (int i = len - 1;i >= 0;i--) {
                if (lpstr[i] == _T('.')) {
                    file_name.Delete(i, len - i);
                    file_ext = lpstr;
                    return std::pair<CString, CString>(file_name, file_ext);
                }
            }

            return std::pair<CString, CString>();
        }
    }//namespace path

#ifdef UNICODE
    typedef std::wstring STD_STRING;
#else
    typedef std::string STD_STRING;
#endif

    template <class T>
    struct record_t
    {
        T root;
        std::list<T> dirs;
        std::list<T> files;
    };

    template <class T>
    bool list_all_files(const T path, std::list<record_t<T> >* lst_records)
    {
        T top_dir = path;
        top_dir += _T("\\*.*");

        //list all files
        WIN32_FIND_DATA find_data;
        memset(&find_data, 0, sizeof(WIN32_FIND_DATA));
        HANDLE h = FindFirstFile(c_str(top_dir), &find_data);
        if (h == INVALID_HANDLE_VALUE) {
            return false;
        }

        record_t<T> record;
        record.root = path;

        do {
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (_tcscmp(find_data.cFileName, _T(".")) == 0) {
                    //skip
                } else if (_tcscmp(find_data.cFileName, _T("..")) == 0) {
                    //skip
                } else {
                    record.dirs.push_back(find_data.cFileName);
                }
            } else {
                record.files.push_back(find_data.cFileName);
            }
        } while (FindNextFile(h, &find_data));

        FindClose(h);
        h = INVALID_HANDLE_VALUE;

        lst_records->push_back(record);

        //list subdir
        for (auto p = record.dirs.begin();p != record.dirs.end();p++) {
            T sub_dir = path;
            sub_dir += _T("\\");
            sub_dir += *p;
            list_all_files(sub_dir, lst_records);
        }

        return true;
    }

    typedef std::list<record_t<STD_STRING> > result_t;
    result_t* walk(STD_STRING top_dir)
    {
        result_t* lst_result = new result_t();

        if (list_all_files(top_dir, lst_result)) {
            return lst_result;
        }

        return NULL;
    }
}//namespace os

#endif

