/* Copyright 2013-2014  Patrick Spendrin <ps_ml@gmx.de>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "kcwinjector.h"
#include "kcwdebug.h"

#include <TlHelp32.h>

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
    HANDLE remoteThread = CreateRemoteThread(m_destProcess, NULL, 0, fnLoadLibrary, mem, 0, NULL);
    DWORD retCode = WaitForSingleObject(remoteThread, INFINITE);
    VirtualFreeEx(m_destProcess, mem, 0, MEM_RELEASE);
    if(retCode != WAIT_OBJECT_0) {
        KcwDebug() << "failed to load dll into process" << GetProcessId(m_destProcess);
        return false;
    }

    HANDLE ths = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetProcessId(m_destProcess));
    if(ths == NULL) {
        KcwDebug() << "creating snapshot failed to find modules";
        return false;
    }

    MODULEENTRY32 me32;
    ZeroMemory(&me32, sizeof(MODULEENTRY32));
    me32.dwSize = sizeof(MODULEENTRY32);

    if(!Module32First(ths, &me32)) {
        KcwDebug() << "couldn't find first module!";
        CloseHandle(ths);
        return false;
    }

    bool retVal = false;

    do {
        if(wcscmp(m_dllPath.c_str(), me32.szExePath) == 0) {
            m_baseAddress = me32.modBaseAddr;
            retVal = true;
            break;
        }
    } while(Module32Next(ths, &me32));

    CloseHandle(ths);
    return retVal;
}

void* KcwInjector::baseAddress() const {
    return m_baseAddress;
}
