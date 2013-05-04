#ifndef LIBPLATFORM_HPP_E860D6C0_B49A_11E2_B328_005056C00008_INCLUDED_
#define LIBPLATFORM_HPP_E860D6C0_B49A_11E2_B328_005056C00008_INCLUDED_

namespace platform
{
    struct WinVersion
    {
        int major;
        int minor;
        int build;
    };

    //DWORD ver_digit = ((ver.major & 0xFF) << 8) | (ver.minor & 0xFF);
    //if (ver_digit < _WIN32_WINNT_WIN7) ...
    static WinVersion winversion()
    {
        WinVersion ver;
        DWORD osver = GetVersion();

        ver.major = osver & 0x00FF;
        ver.minor = (osver >> 8) & 0x00FF;
        ver.build = (osver >> 16) & 0xFFFF;

        return ver;
    }

    static DWORD GET_WIN32_WINNT()
    {
        WinVersion ver = winversion();
        return ((ver.major & 0xFF) << 8) | (ver.minor & 0xFF);
    }

    static CString version()
    {
        WinVersion ver = winversion();
        CString msg;
        msg.Format(_T("%d.%d.%d"), ver.major, ver.minor, ver.build);
        return msg;
    }
}//namespace platform

#endif

