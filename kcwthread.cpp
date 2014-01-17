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
#include "kcwthread.h"

#include <cstdio>
#include <process.h>

#include "kcwdebug.h"

// default value for exitEventHandle is NULL, in that case we need to set a system wide unique name for the event
KcwThread::KcwThread(HANDLE exitEventHandle) : KcwEventLoop() {
    if(exitEventHandle != NULL) {
        setExitEvent(exitEventHandle);
    }
    m_thread = ::CreateThread(NULL, 0, monitorThreadStatic, reinterpret_cast<void*>(this), CREATE_SUSPENDED, NULL);
}

KcwThread::~KcwThread() {
//     KcwDebug() << "leaving thread!";
}

// start the thread by resuming it
void KcwThread::start() {
    ::ResumeThread(m_thread);
}

void KcwThread::pause() {
    ::SuspendThread(m_thread);
}

// a static helper function that is called by the operating system
DWORD WINAPI KcwThread::monitorThreadStatic(LPVOID lpParameter) {
    if(lpParameter == 0) {
//        KcwDebug() << "monitorThreadStatic called with argument:" << lpParameter;
        return 0;
    }
    KcwThread* pKcwThread = reinterpret_cast<KcwThread*>(lpParameter);
//    KcwDebug() << "monitorThreadStatic called with argument:" << pKcwThread;

    const unsigned ret = pKcwThread->monitorThread();
    return ret;
}

// an internal function used to access private data members (to signal the event)
DWORD KcwThread::monitorThread() {
    DWORD dwThreadResult = 0;
    dwThreadResult = run();
//     KcwDebug() << "eventloop for thread ended!";
    return dwThreadResult;
}

// this is the default implementation
DWORD KcwThread::run() {
    return KcwEventLoop::exec();
}
