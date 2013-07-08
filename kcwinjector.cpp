#include "kcwinjector.h"
#include "kcwdebug.h"

KcwInjector::KcwInjector()
 : m_destProcess(NULL),
   m_destThread(NULL) {
}

void KcwInjector::setDestinationProcess(HANDLE proc, HANDLE thread) {
    m_destProcess = proc;
    m_destThread = thread;
}

void KcwInjector::setInjectionDll(std::wstring dllPath) {
    m_dllPath = dllPath;
}

bool KcwInjector::inject() {
    void* mem = NULL;
    size_t memLen = 0;
    bool retVal = true;
    LPTHREAD_START_ROUTINE fnLoadLibrary = NULL;

    if(m_destProcess == NULL || m_destThread == NULL) {
        KcwDebug() << "destination process or thread are empty: process"
                   << m_destProcess << "thread" << m_destThread;
        return false;
    }

    if (::GetFileAttributes(m_dllPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        KcwDebug() << "invalid file attributes for file" << m_dllPath.c_str();
        return false;
    }

    memLen = (m_dllPath.length() + 1) * sizeof(wchar_t);
    mem = VirtualAllocEx(m_destProcess, NULL, memLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    fnLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryW");

    WriteProcessMemory(m_destProcess, mem, m_dllPath.c_str(), (m_dllPath.length() + 1)*sizeof(wchar_t), NULL);
    HANDLE remoteThread = CreateRemoteThread(m_destProcess, NULL, NULL, fnLoadLibrary, mem, NULL, NULL);
    DWORD retCode = WaitForSingleObject(remoteThread, INFINITE);
    if(retCode != WAIT_OBJECT_0) {
        retVal = false;
    }
    // FIXME: exit code on 64 bit is 32 bit, so the baseAddress is truncated. we need to find a different way to get that information
    GetExitCodeThread(remoteThread, &m_baseAddress);
    VirtualFreeEx(m_destProcess, mem, 0, MEM_RELEASE);
    return retVal;
}

DWORD KcwInjector::baseAddress() const {
    return m_baseAddress;
}
