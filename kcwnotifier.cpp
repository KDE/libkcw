#include "kcwnotifier.h"

#include "kcwdebug.h"

KcwNotifier::KcwNotifier()
: m_event(NULL) {
}

KcwNotifier::KcwNotifier(const std::wstring& strName)
: m_event(NULL)
, m_name(strName) {
}

KcwNotifier::~KcwNotifier() {
    close();
}

int KcwNotifier::open() {
    int ret = -1;
    m_event = OpenEvent(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, m_name.c_str());
    DWORD dw = GetLastError();

    // in case the event doesn't exist yet, we get a file not found error, thus we try to create that event
    if(m_event  == NULL && dw == 2 /* file not found */) {
        WCHAR* buf = NULL;

        FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (WCHAR*)&buf,
            0, NULL );

//         KcwDebug() << "Last error when opening event:" << dw << "resulting error message:" << (const wchar_t*)buf;
        LocalFree(buf);

        m_event = CreateEvent(NULL, FALSE, FALSE, m_name.c_str());
        dw = GetLastError();
        if(m_event == NULL) {
            buf = NULL;
            FormatMessageW(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (WCHAR*)&buf,
                0, NULL );
//             KcwDebug() << "Last error when creating event:" << dw << "resulting error message:" << (const wchar_t*)buf;
            LocalFree(buf);
            exit(-1);
        }
    }
    return 0;
}

int KcwNotifier::open(const std::wstring& strName) {
    setNotifierName(strName);
    return open();
}

bool KcwNotifier::opened() const {
    return m_event != NULL;
}

void KcwNotifier::close() {
    if(m_event != NULL) {
        CloseHandle(m_event);
        m_event = NULL;
    }
}
void KcwNotifier::setNotifierName(const std::wstring& strName) {
    m_name = strName;
}

std::wstring KcwNotifier::notifierName() const {
    return m_name;
}

void KcwNotifier::notify() {
    SetEvent(m_event);
}

HANDLE KcwNotifier::handle() {
    return m_event;
}

KcwNotifier::operator HANDLE() {
    return m_event;
}