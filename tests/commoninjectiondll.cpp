#include "kcwsharedmemory.h"
#include "kcwdebug.h"
#include <windows.h>

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */ ) {
    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
        {
            KcwSharedMemory<int> shmem(L"injectortest", 1, false);
            KcwSharedMemory<WCHAR> shmemvar(L"injectortestvar", 9, false);
            *shmem += 1;
            memcpy(shmemvar.data(), _wgetenv(L"MYTEST"), 9 * sizeof(WCHAR));
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            break;
        }
    };
    return TRUE;
}
