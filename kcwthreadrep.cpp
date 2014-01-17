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
#include "kcwthreadrep.h"

#include <tlhelp32.h>


KcwThreadRep::KcwThreadRep()
 :  m_process(NULL),
    m_thread(NULL) {
}

KcwThreadRep::KcwThreadRep(HANDLE thread) { KcwThreadRep(GetCurrentProcess(), thread); }

KcwThreadRep::KcwThreadRep(HANDLE app, HANDLE thread)
 :  m_process(app),
    m_thread(thread) {
}

KcwThreadRep::KcwThreadRep(int threadid) { KcwThreadRep(GetCurrentProcess(), getHandleForThreadId(threadid)); }

KcwThreadRep::KcwThreadRep(int pid, int threadid) { KcwThreadRep(getHandleForPid(pid), getHandleForThreadId(threadid)); }

void KcwThreadRep::attachThread(HANDLE thread) {
    attachAppThread(GetCurrentProcess(), thread);
}

void KcwThreadRep::attachAppThread(HANDLE app, HANDLE thread) {
    m_process = app;
    m_thread = thread;
}

void KcwThreadRep::attachThreadId(int threadid) {
    attachAppThread(GetCurrentProcess(), getHandleForThreadId(threadid));
}

bool KcwThreadRep::resume() {
    return ResumeThread(m_thread) == 1;
}

bool KcwThreadRep::suspend() {
    return SuspendThread(m_thread) >= 0;
}

HANDLE KcwThreadRep::threadHandle() const {
    return m_thread;
}

HANDLE KcwThreadRep::processHandle() const {
    return m_process;
}

std::vector<KcwThreadRep> KcwThreadRep::threads(HANDLE app) {
    return threads(GetProcessId(app));
}

std::vector<KcwThreadRep> KcwThreadRep::threads(int pid) {
    std::vector<KcwThreadRep> ret = std::vector<KcwThreadRep>();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if(snapshot != INVALID_HANDLE_VALUE) {
        THREADENTRY32 threadentry;
        threadentry.dwSize = sizeof(threadentry);
        if(Thread32First(snapshot, &threadentry)) {
            do {
                if(threadentry.th32OwnerProcessID == pid) {
                    ret.push_back(KcwThreadRep(pid, threadentry.th32ThreadID));
                }
            } while(Thread32Next(snapshot, &threadentry));
        }
    }
    return ret;
}

HANDLE KcwThreadRep::getHandleForPid(int pid) {
//    return OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
}

HANDLE KcwThreadRep::getHandleForThreadId(int threadid) {
    return OpenProcess(THREAD_ALL_ACCESS, FALSE, threadid);
}
