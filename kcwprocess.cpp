#include "kcwprocess.h"
#include "kcwdebug.h"

KcwProcess::KcwProcess(std::wstring execPath)
  : m_cmd(execPath),
    m_isRunning(false),
    m_isStartedAsHidden(false),
    m_startupFlags(CREATE_NEW_CONSOLE|CREATE_SUSPENDED),
    m_isStartedAsPaused(true),
    m_environment(KcwProcessEnvironment::getCurrentEnvironment()) {
    m_stdHandles[KCW_STDIN_HANDLE] = 0;
    m_stdHandles[KCW_STDOUT_HANDLE] = 0;
    m_stdHandles[KCW_STDERR_HANDLE] = 0;
}

/**
* @todo this is not usable so far.
*/
KcwProcess::KcwProcess(int pid)
  : m_isRunning(true),
    m_startupFlags(CREATE_NEW_CONSOLE|CREATE_SUSPENDED),
    m_isStartedAsHidden(false),
    m_isStartedAsPaused(true),
    m_environment(KcwProcessEnvironment::getCurrentEnvironment()) {
    m_stdHandles[KCW_STDIN_HANDLE] = 0;
    m_stdHandles[KCW_STDOUT_HANDLE] = 0;
    m_stdHandles[KCW_STDERR_HANDLE] = 0;
}

KcwProcess::KcwProcess()
  : m_isRunning(false),
    m_startupFlags(CREATE_NEW_CONSOLE|CREATE_SUSPENDED),
    m_isStartedAsHidden(false),
    m_isStartedAsPaused(true),
    m_environment(KcwProcessEnvironment::getCurrentEnvironment()) {
    m_stdHandles[KCW_STDIN_HANDLE] = 0;
    m_stdHandles[KCW_STDOUT_HANDLE] = 0;
    m_stdHandles[KCW_STDERR_HANDLE] = 0;
}

void KcwProcess::setStdHandle(HANDLE hdl, KCW_STREAM_TYPE type) {
    m_stdHandles[type] = hdl;
}

HANDLE KcwProcess::stdHandle(KCW_STREAM_TYPE type) const {
    return m_stdHandles[type];
}

void KcwProcess::setIsStartedAsPaused(bool isPaused) {
    m_isStartedAsPaused = isPaused;
}

void KcwProcess::setCmd(const std::wstring& _cmd) {
    m_cmd = _cmd;
}

void KcwProcess::setStartupFlags(int stFlags) {
    m_startupFlags = stFlags;
}

bool KcwProcess::start() {
    STARTUPINFOW siWow;

    ::ZeroMemory(&siWow, sizeof(STARTUPINFOW));

    siWow.cb            = sizeof(STARTUPINFOW);
    if(m_stdHandles[KCW_STDIN_HANDLE]) {
        siWow.hStdInput = m_stdHandles[KCW_STDIN_HANDLE];
        siWow.dwFlags   = STARTF_USESTDHANDLES;
    } else {
        siWow.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    }
    if(m_stdHandles[KCW_STDOUT_HANDLE]) {
        siWow.hStdOutput = m_stdHandles[KCW_STDOUT_HANDLE];
        siWow.dwFlags    = STARTF_USESTDHANDLES;
    } else {
        siWow.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    }
    if(m_stdHandles[KCW_STDERR_HANDLE]) {
        siWow.hStdError = m_stdHandles[KCW_STDERR_HANDLE];
        siWow.dwFlags   = STARTF_USESTDHANDLES;
    } else {
        siWow.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    }
    DWORD dwStartupFlags = m_startupFlags;
//    DWORD dwStartupFlags = CREATE_SUSPENDED;
//  | DETACHED_PROCESS; // the detached process won't work
    siWow.dwFlags       |= STARTF_USESHOWWINDOW;
    dwStartupFlags      |= CREATE_UNICODE_ENVIRONMENT;
    siWow.wShowWindow   = (m_isStartedAsHidden) ? SW_HIDE : SW_SHOW;

    WCHAR* envBuffer;
    DWORD envLen = 1;  // the terminating 0 character, envLen is counted in characters, not in bytes

    // calculate the length of the required buffer
    for(KcwProcessEnvironment::iterator it = m_environment.begin(); it != m_environment.end(); ++it) {
//         KcwDebug() << "sizes:" << it->first.size() << it->second.size() << envLen;
        envLen += it->first.size() + it->second.size() + 2; // envLen is counted in characters, not in bytes
    }

    envBuffer = new WCHAR[envLen * sizeof(WCHAR)];
    ZeroMemory(envBuffer, envLen * sizeof(WCHAR));
    WCHAR* curEnv = envBuffer;
    for(KcwProcessEnvironment::iterator it = m_environment.begin(); it != m_environment.end(); ++it) {
        WCHAR* tmp = curEnv;
        memcpy(curEnv, it->first.c_str(), it->first.size() * sizeof(WCHAR));
        curEnv += it->first.size();
        *(curEnv++) = L'=';
        memcpy(curEnv, it->second.c_str(), it->second.size() * sizeof(WCHAR));
        curEnv += it->second.size();
        *(curEnv++) = L'\0';
    }
    *(curEnv++) = L'\0';

    WCHAR* iwd = NULL;
    if(m_initialWorkingDirectory.size() > 0) iwd = const_cast<WCHAR*>(m_initialWorkingDirectory.c_str());

    PROCESS_INFORMATION procInfo;
    if(!::CreateProcessW(
            NULL,
            const_cast<WCHAR*>(m_cmd.c_str()),
            NULL,
            NULL,
            FALSE,
            dwStartupFlags,
            envBuffer,
            iwd,
            &siWow,
            &procInfo))
    {
        DWORD dw = GetLastError();
        WCHAR* lpMsgBuf = NULL;

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            lpMsgBuf,
            0, NULL);

        OutputDebugStringW(lpMsgBuf);
        LocalFree(lpMsgBuf);
        return false;
    }
    m_threadRep.attachAppThread(procInfo.hProcess, procInfo.hThread);
    return true;
}

bool KcwProcess::resume() {
    return m_threadRep.resume();
}

int KcwProcess::startupFlags() const {
    return m_startupFlags;
}

HANDLE KcwProcess::process() const {
    return m_threadRep.processHandle();
}

HANDLE KcwProcess::thread() const {
    return m_threadRep.threadHandle();
}

KcwThreadRep KcwProcess::threadRep() const {
    return m_threadRep;
}


bool KcwProcess::isRunning() const {
    return m_isRunning;
}

int KcwProcess::pid() const {
    return GetProcessId(m_threadRep.processHandle());
}

std::wstring KcwProcess::cmd() const {
    return m_cmd;
}

void KcwProcess::setStartupAsHidden(bool x) {
    m_isStartedAsHidden = x;
}

bool KcwProcess::startupAsHidden() const {
    return m_isStartedAsHidden;
}

int KcwProcess::exitCode() const {
    DWORD result = 0;
    if(!GetExitCodeProcess(m_threadRep.processHandle(), &result)) {
        return -1;
    } else {
        return result;
    }
}

void KcwProcess::setInitialWorkingDirectory(const std::wstring& iwd) {
    m_initialWorkingDirectory = iwd;
}

std::wstring KcwProcess::initialWorkingDirectory() const {
    return m_initialWorkingDirectory;
}


void KcwProcess::quit() {
    TerminateProcess(m_threadRep.processHandle(), 0);
}

void KcwProcess::setStartupEnvironment(KcwProcessEnvironment env) {
    m_environment = env;
}

KcwProcess::KcwProcessEnvironment KcwProcess::KcwProcessEnvironment::getCurrentEnvironment() {
    WCHAR* envBlock = GetEnvironmentStrings();
    KcwProcessEnvironment ret;
    while(*envBlock != L'\0') {
        std::wstring tmp = envBlock;
        size_t eq = tmp.find(L'=');
        ret[tmp.substr(0, eq)] = tmp.substr(eq + 1);
        envBlock += tmp.size() + 1;
    }
    return ret;
}
