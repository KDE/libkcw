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
#include "kcwprocess.h"
#include "kcwapp.h"
#include "kcwinjector.h"
#include "kcwsharedmemory.h"

#define KcwTestAssert(argument, output) if(!(argument)) { OutputDebugString(output); exit(1); }

std::wstring getModulePath(HMODULE hModule) {
    WCHAR szModulePath[MAX_PATH + 1];
    ::ZeroMemory(szModulePath, (MAX_PATH + 1)*sizeof(WCHAR));

    ::GetModuleFileName(hModule, szModulePath, MAX_PATH);

    std::wstring strPath(szModulePath);

    return strPath.substr(0, strPath.rfind('\\'));
}


int main(int argc, char**argv) {
    KcwApp app;
    KcwProcess proc(L"cmd.exe");
    KcwInjector injector, injector2;
    KcwSharedMemory<int> shmem;
    KcwSharedMemory<WCHAR> shmemvar;
    HANDLE timer = CreateWaitableTimer(NULL, FALSE, NULL);
    int retval;
    KcwProcess::KcwProcessEnvironment env = KcwProcess::KcwProcessEnvironment::getCurrentEnvironment();

    env[L"MYTEST"] = L"myblub__";
    proc.setStartupEnvironment(env);

    injector2.setInjectionDll(getModulePath(NULL) + L"\\commoninjector.dll");
    KcwTestAssert((injector2.inject() == false), L"KcwInjector didn't fail when no process was set");

    if(!proc.start()) {
        printf("process failed to start!");
        return -1;
    }

    shmem.create(L"injectortest");
    *shmem = 1;
    shmemvar.create(L"injectortestvar", 9);
    injector.setDestinationProcess(proc.process(), proc.thread());
    injector.setInjectionDll(getModulePath(NULL) + L"\\commoninjector.dll");
    if(!injector.inject()) {
        printf("failed to inject dll");
        return -1;
    }

    app.addCallback(proc.process());
    LARGE_INTEGER li;
    li.QuadPart = -20 * 1000000LL; // 2 seconds
    SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE);
    app.addCallback(timer, CB(KcwProcess::quit), &proc);
    proc.resume();
    retval = app.exec();
    KcwTestAssert((*shmem == 2), L"injected dll didn't run");

    WCHAR buf[9];
    memcpy(buf, shmemvar.data(), sizeof(WCHAR) * 9);
    KcwTestAssert((wcscmp(buf, L"myblub__") == 0), L"envvar couldn't be found in process");

    return retval;
}