#include <vector>

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
}

KcwEventLoop::~KcwEventLoop() {
//     KcwDebug() << "Leaving event loop #" << m_eventLoopId;
}

void KcwEventLoop::addCallback(HANDLE hndl, HANDLE event) {
//     KcwDebug() << "add event handle #" << m_handles.size() <<  L"in eventLoop" << m_eventLoopId << L"value:" << hndl;
    m_handles.push_back(hndl);
    m_objects.push_back(event);
    m_callbacks.push_back(handleCallback);
}

void KcwEventLoop::addCallback(HANDLE hndl, eventCallback cllbck, void *callbackObject) {
//     KcwDebug() << "add callback handle #" << m_handles.size() <<  L"in eventLoop" << m_eventLoopId << L"value:" << hndl;
//     KcwDebug() << "callback object is:" << (int)callbackObject << "this eventloop is:" << this;
    m_handles.push_back(hndl);
    if(callbackObject) m_objects.push_back(callbackObject);
    else m_objects.push_back(this);
    m_callbacks.push_back(cllbck);
}

void KcwEventLoop::removeCallback(HANDLE hndl, eventCallback cllbck) {
//     KcwDebug() << "removing callback for handle" << hndl << "in eventLoop #" << m_eventLoopId;
    for(int i = 1; i < m_handles.size(); i++) {
        if(m_handles[i] == hndl) {
            if((cllbck != NULL && cllbck == m_callbacks[i]) || cllbck == 0) {
                m_removeCallbacks.push_back(i);
            }
        }
    }
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

bool KcwEventLoop::callForObject(int objNum) {
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
    int handleSize = locHandles.size();
//     KcwDebug() << "checking for #" << handleSize << " handles";
    for(int i = 0; i < handleSize; i++) {
        if(!GetHandleInformation(locHandles.at(i), &dwHandleInfo)) {
            KcwDebug() << "Handle #" << i << "is broken in process" << dwProcessId;
            LeaveCriticalSection(&m_criticalSection);
            return -1;
        }
    };

    HANDLE *begin = &locHandles.front();
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

        int i = 1;
        bool quitNow = false;
        for(; i < handleSize; i++) {
            if(dwWaitRes == WAIT_OBJECT_0 + i) {
//                 KcwDebug() << "calling cb for " << i << " in eventLoop #" << m_eventLoopId;
                quitNow = callForObject(i);
                break;
            }
        };
        if(quitNow) break;

        if(dwWaitRes == WAIT_TIMEOUT) continue;

        for(int j = 0; j < m_removeCallbacks.size(); j++) {
            m_handles.erase(m_handles.begin() + j);
            m_objects.erase(m_objects.begin() + j);
            m_callbacks.erase(m_callbacks.begin() + j);
            handleSize = m_handles.size();
        }
        m_removeCallbacks.erase(m_removeCallbacks.begin(), m_removeCallbacks.end());
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
