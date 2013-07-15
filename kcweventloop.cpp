#include <vector>

#include "kcweventloop.h"
#include "kcwsharedmemory.h"
#include "kcwdebug.h"

KcwSharedMemory<int> KcwEventLoop::s_globalEventLoopCounter;

KcwEventLoop::KcwEventLoop(HANDLE eventHandle)
 :  m_refreshInterval(10),
    m_eventLoopId(getUniqueCounter()) {
    InitializeCriticalSection(&m_criticalSection);
    EnterCriticalSection(&m_criticalSection);

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
    LeaveCriticalSection(&m_criticalSection);
}

KcwEventLoop::~KcwEventLoop() {
}

void KcwEventLoop::addCallback(HANDLE hndl, HANDLE event) {
    EnterCriticalSection(&m_criticalSection);
//     KcwDebug() << "add event handle #" << (m_handles.size() + 1) <<  L"in eventLoop" << m_eventLoopId << L"value:" << hndl;
    m_handles.push_back(hndl);
    m_objects.push_back(event);
    m_callbacks.push_back(handleCallback);
    LeaveCriticalSection(&m_criticalSection);
}

void KcwEventLoop::addCallback(HANDLE hndl, eventCallback cllbck, void *callbackObject) {
    EnterCriticalSection(&m_criticalSection);
//     KcwDebug() << "add callback handle #" << (m_handles.size() + 1) <<  L"in eventLoop" << m_eventLoopId << L"value:" << hndl;
//     KcwDebug() << "callback object is:" << (int)callbackObject << "this eventloop is:" << this;
    m_handles.push_back(hndl);
    if(callbackObject) m_objects.push_back(callbackObject);
    else m_objects.push_back(this);
    m_callbacks.push_back(cllbck);
    LeaveCriticalSection(&m_criticalSection);
}

void KcwEventLoop::quit() {
    DWORD dwProcessId = ::GetCurrentProcessId();
    KcwDebug() << "quit was called in process" << dwProcessId;
    EnterCriticalSection(&m_criticalSection);
    SetEvent(m_eventHandle);
    LeaveCriticalSection(&m_criticalSection);
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

void KcwEventLoop::callForObject(int objNum) {
    if(m_callbacks[objNum] != NULL) {
//         KcwDebug() << "calling callback for event #" << objNum << "in eventloop #" << m_eventLoopId << "of process" << dwProcessId;
        eventCallback callback = m_callbacks[objNum];
        void *arg = m_objects[objNum];
        LeaveCriticalSection(&m_criticalSection);
//         KcwDebug() << "argument:" << arg;
        callback(arg);
        EnterCriticalSection(&m_criticalSection);
    } else {
//         KcwDebug() << "calling quit for event #" << objNum << "in eventloop #" << m_eventLoopId << "of process" << dwProcessId;
        LeaveCriticalSection(&m_criticalSection);
        quit();
        EnterCriticalSection(&m_criticalSection);
    }
}

int KcwEventLoop::exec() {
    DWORD dwHandleInfo = 0, dwWaitRes = 0, dwProcessId = ::GetCurrentProcessId();

    std::vector<HANDLE> locHandles(m_handles);
    EnterCriticalSection(&m_criticalSection);
    const int handleSize = locHandles.size();
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
//        LeaveCriticalSection(&m_criticalSection);
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

//            EnterCriticalSection(&m_criticalSection);
            KcwDebug() << "eventLoop wait failed!" << endl << "pid:" << dwProcessId << "#handles:" << locHandles.size() << "result:" << dwWaitRes << (const wchar_t*)lpMsgBuf << endl;
            break;
        }

//        EnterCriticalSection(&m_criticalSection);
        if(dwWaitRes == WAIT_TIMEOUT) continue;

        int i = 0;
        for(; i < handleSize; i++) {
            if(dwWaitRes == WAIT_OBJECT_0 + i) {
                callForObject(i);
                break;
            }
        };
        for(int j = i + 1; j < handleSize; j++) {
            if(m_objects[i] == m_objects[j]) {
                callForObject(j);
            }
        };
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
