#ifndef LIBLOADIMAGE_HPP_8FEBB430_B3F3_11E2_87BB_005056C00008_INCLUDED_
#define LIBLOADIMAGE_HPP_8FEBB430_B3F3_11E2_87BB_005056C00008_INCLUDED_

BOOL LoadImageFromResourceID(CImage* image, UINT id, TCHAR* type = _T("PNG"))
{
    //find resource
    HINSTANCE h_instance = AfxGetResourceHandle();
    HRSRC h_resource = ::FindResource (h_instance, MAKEINTRESOURCE(id), type);
    if (!h_resource) {
        return FALSE;
    }

    //load resource into memory
    DWORD len = SizeofResource(h_instance, h_resource);
    BYTE* p_resource = (BYTE*)LoadResource(h_instance, h_resource);
    if (!p_resource) {
        return FALSE;
    }

    //allocate global memory on which to create stream
    HGLOBAL h_global_memory = GlobalAlloc(GMEM_FIXED, len);

    //lock and copy resource to buffer
    BYTE* buffer = (BYTE*)GlobalLock(h_global_memory);
    memcpy(buffer, p_resource, len);

    IStream* mem_stream;
    CreateStreamOnHGlobal(h_global_memory, FALSE, &mem_stream);

    //load from stream
    image->Load(mem_stream);

    //clean
    GlobalUnlock(h_global_memory);
    mem_stream->Release();
    FreeResource(p_resource);

    return TRUE;
}

#endif

