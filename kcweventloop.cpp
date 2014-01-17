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
#include <vector>
#include <algorithm>

#include "kcweventloop.h"
#include "kcwsharedmemory.h"
#include "kcwdebug.h"

KcwSharedMemory<int> KcwEventLoop::s_globalEventLoopCounter;

KcwEventLoop::KcwEventLoop(HANDLE eventHandle)
 :  m_refreshInterval(10),
    m_eventLoopId(getUniqueCounter()) {
    InitializeCriticalSection(&m_criticalSection);

    // the event handle is used to signal that this eventloop should simply close itself.
    if(eventHandle != NULL) {
        m_eventHandle = eventHandle;
//         KcwDebug() << "using a different event handle";
    } else {
        WCHAR tmp[1024];
        wsprintf(tmp, L"KcwEventLoop-%i", m_eventLoopId);
//         KcwDebug() << "using eventLoop event:" << (const wchar_t*)tmp;
        m_eventHandle = ::CreateEvent(NULL, FALSE, FALSE, tmp);
    }

    m_handles.push_back(m_eventHandle);
    m_objects.push_back(NULL);
    m_callbacks.push_back(NULL);
    m_singleCall.push_back(false);
}

KcwEventLoop::~KcwEventLoop() {
//     KcwDebug() << "Leaving event loop #" << m_eventLoopId;
}

void KcwEventLoop::addCallback(HANDLE hndl, eventCallback cllbck, void *callbackObject, bool singleCall) {
//     KcwDebug() << "add callback handle #" << m_handles.size() <<  L"in eventLoop" << m_eventLoopId << L"value:" << hndl;
//     KcwDebug() << "callback object is:" << (int)callbackObject << "this eventloop is:" << this;
    for(unsigned i = 0; i < m_handles.size(); i++) {
        if(m_handles[i] == hndl) {
            KcwDebug() << "Error! trying to add the same handle twice to this eventloop!";
            KcwDebug() << "Handle in question is:" << hndl;
            return;
        }
    }
    m_handles.push_back(hndl);
    if(callbackObject) m_objects.push_back(callbackObject);
    else m_objects.push_back(this);
    m_callbacks.push_back(cllbck);
    m_singleCall.push_back(singleCall);
}

void KcwEventLoop::removeCallback(HANDLE hndl, eventCallback cllbck) {
//     KcwDebug() << "removing callback for handle" << hndl << "in eventLoop #" << m_eventLoopId;
    for(unsigned i = 1; i < m_handles.size(); i++) {
        if(m_handles[i] == hndl && ((cllbck != NULL && cllbck == m_callbacks[i]) || cllbck == 0)) {
            removeCallback(i);
            return;
        }
    }
}

void KcwEventLoop::removeCallback(unsigned n) {
    for(unsigned i = 0; i < m_removeCallbacks.size(); i++) {
        if(m_removeCallbacks[i] == n) return;
    }
    m_removeCallbacks.push_back(n);
}

unsigned KcwEventLoop::cleanupCallbacks() {
    // first sort the callbacks, then reverse the order, so that removing that callback doesn't change the order of 
    std::sort(m_removeCallbacks.begin(), m_removeCallbacks.end());
    std::reverse(m_removeCallbacks.begin(), m_removeCallbacks.end());

    for(unsigned j = 0; j < m_removeCallbacks.size(); j++) {
        int toErase = m_removeCallbacks[j];
        m_handles.erase(m_handles.begin() + toErase);
        m_objects.erase(m_objects.begin() + toErase);
        m_callbacks.erase(m_callbacks.begin() + toErase);
        m_singleCall.erase(m_singleCall.begin() + toErase);
    }
    m_removeCallbacks.erase(m_removeCallbacks.begin(), m_removeCallbacks.end());
    return m_handles.size();
}

void KcwEventLoop::quit() {
    DWORD dwProcessId = ::GetCurrentProcessId();
//     KcwDebug() << "quit was called in process" << dwProcessId << " in eventLoop" << m_eventLoopId;
    SetEvent(m_eventHandle);
}

void KcwEventLoop::setRefreshInterval(int secs) {
//    KcwDebug() << "setting refresh interval";
    EnterCriticalSection(&m_criticalSection);
    m_refreshInterval = secs;
    LeaveCriticalSection(&m_criticalSection);
}

int KcwEventLoop::refreshInterval() const {
    return m_refreshInterval;
}

void KcwEventLoop::setExitEvent(HANDLE event) {
//    KcwDebug() << "setting exit event";
    EnterCriticalSection(&m_criticalSection);
    m_eventHandle = event;
    m_handles.at(0) = event;
    LeaveCriticalSection(&m_criticalSection);
}

HANDLE KcwEventLoop::exitEvent() {
    return m_eventHandle;
}

int KcwEventLoop::eventLoopId() const {
    return m_eventLoopId;
}

bool KcwEventLoop::callForObject(unsigned objNum) {
    if(m_callbacks[objNum] != NULL) {
//         KcwDebug() << "calling callback for event #" << objNum << "in eventloop #" << m_eventLoopId;
        eventCallback callback = m_callbacks[objNum];
        void *arg = m_objects[objNum];
        LeaveCriticalSection(&m_criticalSection);
//         KcwDebug() << "argument:" << arg;
        callback(arg);
        EnterCriticalSection(&m_criticalSection);
        return false;
    } else {
//         KcwDebug() << "calling quit for event #" << objNum << "in eventloop #" << m_eventLoopId << "of process" << dwProcessId;
        LeaveCriticalSection(&m_criticalSection);
        quit();
        EnterCriticalSection(&m_criticalSection);
        return true;
    }
}

int KcwEventLoop::exec() {
    DWORD dwHandleInfo = 0, dwWaitRes = 0, dwProcessId = ::GetCurrentProcessId();

    EnterCriticalSection(&m_criticalSection);
    std::vector<HANDLE> locHandles(m_handles);
    unsigned handleSize = locHandles.size();
//     KcwDebug() << "checking for #" << handleSize << " handles";
    for(unsigned i = 0; i < handleSize; i++) {
        if(!GetHandleInformation(locHandles.at(i), &dwHandleInfo)) {
            KcwDebug() << "Handle #" << i << "is broken in process" << dwProcessId;
            LeaveCriticalSection(&m_criticalSection);
            return -1;
        }
    };

    HANDLE *begin = &locHandles.front();
    MSG msg;
    while ((dwWaitRes = ::WaitForMultipleObjects(handleSize, begin, FALSE, m_refreshInterval)) != WAIT_OBJECT_0) {
        if(dwWaitRes == WAIT_FAILED) {
            WCHAR lpMsgBuf[1024];
            DWORD dw = GetLastError(); 
            FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                lpMsgBuf,
                0, NULL );

            KcwDebug() << "eventLoop wait failed!" << endl << "pid:" << dwProcessId << "#handles:" << locHandles.size() << "result:" << dwWaitRes << (const wchar_t*)lpMsgBuf << endl;
            break;
        }

        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) {
            DispatchMessage(&msg);
        }

        unsigned i = 1;
        bool quitNow = false;
        for(; i < handleSize; i++) {
            if(dwWaitRes == WAIT_OBJECT_0 + i) {
//                 KcwDebug() << "calling cb for " << i << " in eventLoop #" << m_eventLoopId;
                quitNow = callForObject(i);

                // in case we have a single call, we want to unregister now
                if(m_singleCall[i]) m_removeCallbacks.push_back(i);
                break;
            }
        };

        if(dwWaitRes == WAIT_TIMEOUT) continue;

        handleSize = cleanupCallbacks();

        if(quitNow) break;
    }
    LeaveCriticalSection(&m_criticalSection);
    return 0;
}

void KcwEventLoop::handleCallback(HANDLE obj) {
    SetEvent(obj);
}

int KcwEventLoop::getUniqueCounter() {
    // we need to check that our threadCounter has started
    // KcwSharedMemory<T>::open() and KcwSharedMemory<T>::create()
    // return 0 in case they already have been opened.
    if(s_globalEventLoopCounter.open(L"KcwEventLoopGlobal") != 0) {
        if(s_globalEventLoopCounter.create(L"KcwEventLoopGlobal") != 0) {
            // in case of failure, exit the complete application
            s_globalEventLoopCounter.errorExit();
        } else {
            *s_globalEventLoopCounter = 0;
        }
    }

    // increase the counter by one, currently this is still not thread save
//    KcwDebug() << "opening global eventLoop number" << *s_globalEventLoopCounter;
    return (*s_globalEventLoopCounter)++;
}
