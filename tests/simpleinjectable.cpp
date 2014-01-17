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
#include "simpleinjectable.h"

#include "kcwprocess.h"
#include "kcweventloop.h"
#include "kcwinjector.h"
#include "kcwsharedmemory.h"
#include "kcwdebug.h"

#define KcwTestAssert(argument, output) if(!(argument)) { OutputDebugString(output); exit(1); }

HINSTANCE hModule = NULL;

std::wstring getModulePath(HMODULE hModule) {
    WCHAR szModulePath[MAX_PATH + 1];
    ::ZeroMemory(szModulePath, (MAX_PATH + 1)*sizeof(WCHAR));

    ::GetModuleFileName(hModule, szModulePath, MAX_PATH);

    std::wstring strPath(szModulePath);

    return strPath.substr(0, strPath.rfind('\\'));
}

extern "C" __declspec(dllexport) BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */ ) {
    hModule = hInstance;

    switch(dwReason) {
        case DLL_PROCESS_ATTACH:
        {
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            break;
        }
    };
    return TRUE;
}

DWORD HookService(void *val) {
    KcwDebug() << "HookService was called!";
    return 0;
}

int startConsole() {
    KcwEventLoop loop;
    KcwProcess proc(L"cmd.exe");
    KcwInjector injector;
    HANDLE timer = CreateWaitableTimer(NULL, FALSE, NULL);
    int retval;

    if(!proc.start()) {
        printf("process failed to start!");
        return -1;
    }

    injector.setDestinationProcess(proc.process(), proc.thread());
    injector.setInjectionDll(getModulePath(NULL) + L"\\simpleinjectable.dll");
    if(!injector.inject()) {
        printf("failed to inject dll");
        return -1;
    }

    loop.addCallback(proc.process());
    LARGE_INTEGER li;
    li.QuadPart = -30 * 1000000LL; // 3 seconds
    SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE);
    loop.addCallback(timer, CB(KcwProcess::quit), &proc);

    LPTHREAD_START_ROUTINE pfnThreadRoutine = (LPTHREAD_START_ROUTINE)((char*)injector.baseAddress() + ((char*)HookService - (char*)hModule));
    HANDLE hRemoteThread = CreateRemoteThread(proc.process(), NULL, 0, pfnThreadRoutine, NULL, 0, NULL);

    KcwDebug() << "before the process is resumed!";
    proc.resume();
    retval = loop.exec();
    return retval;
}
