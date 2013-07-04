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

// start the thread by resuming it
void KcwThread::start() {
    ::ResumeThread(m_thread);
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
    KcwEventLoop::quit();
    return dwThreadResult;
}

// this is the default implementation
DWORD KcwThread::run() {
    return KcwEventLoop::exec();
}
