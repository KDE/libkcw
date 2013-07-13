#include <iostream>
#include <string>

#include <windows.h>

#include "kcwdebug.h"

bool KcwDebug::s_enabled = true;
std::locale KcwDebug::s_loc("");

KcwDebug::KcwDebug()
 :  m_maybeSpace( false ),
    m_stringptr( NULL )
{}

KcwDebug::KcwDebug(std::string* ptr)
 :  m_maybeSpace( false ),
    m_stringptr( ptr )
{}

KcwDebug::~KcwDebug() {
    if(s_enabled) {
//        std::cout << m_ss.str();
        OutputDebugStringA(m_ss.str().c_str());
    }
    if(m_stringptr) *m_stringptr = m_ss.str();
}

KcwDebug& KcwDebug::maybeSpaceReference() {
    m_maybeSpace = true;
    return *this;
}

void KcwDebug::spaceIt() {
    if(m_maybeSpace) m_ss << " ";
    m_maybeSpace = false;
}

KcwDebug& KcwDebug::operator<<(ManipFunc manipFunc) {
    return manipFunc(*this);
}

void KcwDebug::setEnabled(bool enable) {
    s_enabled = enable;
}

bool KcwDebug::enabled() {
    return s_enabled;
}

// manipulation function
/**
* writes an end of line character (std::endl) either to the debug console or the string
* used by the KcwDebug object.
*/
KcwDebug& endl(KcwDebug& os) {
    // before endl, we don't need a space
    os.m_maybeSpace = false;
    os.m_ss << std::endl;
    return os;
}

template<> KcwDebug& KcwDebug::operator<<(std::wstring i) {
    spaceIt();
    const wchar_t *wstr = i.c_str();
    char *str = new char[i.length() + 1];
    ZeroMemory(str, (i.length() + 1) * sizeof(char));
    std::use_facet<std::ctype<wchar_t>>(s_loc).narrow(wstr, wstr+wcslen(wstr), '?', str);
    str[wcslen(wstr)] = 0;
    m_ss << str;
    return maybeSpaceReference();
}

template<> KcwDebug& KcwDebug::operator<<(const wchar_t* i) {
    return KcwDebug::operator<<(std::wstring(i));
}
